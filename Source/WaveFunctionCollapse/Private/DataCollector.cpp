// Fill out your copyright notice in the Description page of Project Settings.


#include "DataCollector.h"
#include "RoomBase.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

//JSON includes
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonObjectConverter.h"

// Sets default values
ADataCollector::ADataCollector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Distance = 300.0f;
}

// Called when the game starts or when spawned
void ADataCollector::BeginPlay()
{
	Super::BeginPlay();

	//create json structure in memory
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	UWorld* World = GetWorld();

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[%hs]: found simple actor: %s"), __FUNCTION__, *ItActor->GetName());

		FVector Start = ItActor->GetActorLocation();

		FRoomData CurrentRoom;
		CurrentRoom.RoomClass = ItActor->GetClass();
		
		TArray<TArray<TSubclassOf<ARoomBase>>> RoomAdjacent;
		RoomAdjacent.SetNum(4);
		int32 Index = RoomsData.Find(CurrentRoom);
		if (Index != INDEX_NONE)
		{
			RoomAdjacent[0].Append(RoomsData[Index].Forward);
			RoomAdjacent[1].Append(RoomsData[Index].Back);
			RoomAdjacent[2].Append(RoomsData[Index].Left);
			RoomAdjacent[3].Append(RoomsData[Index].Right);
		}

		TArray<FVector> Direction;
		Direction.SetNum(4);

		Direction[0] = ItActor->GetActorForwardVector();
		Direction[1] = -ItActor->GetActorForwardVector();
		Direction[2] = ItActor->GetActorRightVector();
		Direction[3] = -ItActor->GetActorRightVector();

		for (int i = 0; i < 4; i++)
		{
			Direction[i].Normalize();

			FVector End = Start + (Direction[i] * Distance);

			FHitResult Hit;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(*ItActor);

			bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

			if (bHit)
			{
				UE_LOG(LogTemp, Log, TEXT("Hit %s at ^s"), *Hit.GetActor()->GetName(), *Hit.ImpactPoint.ToString());
				DrawDebugLine(World, Start, End, FColor::Green, true, 100.0f, 0, 1.5f);

				bool bRommExists = RoomAdjacent[i].Contains(Hit.GetActor()->GetClass());
				if (!bRommExists)
				{
					RoomAdjacent[i].Add(Hit.GetActor()->GetClass());
				}
			}
			else
				DrawDebugLine(World, Start, End, FColor::Red, true, 100.0f, 0, 1.5f);

		}

		if (Index != INDEX_NONE)
		{
			//To Do: find better way to work around ue5 2d array limitation to remove the need to constantly empty arrays.
			RoomsData[Index].Forward.Empty();
			RoomsData[Index].Back.Empty();
			RoomsData[Index].Left.Empty();
			RoomsData[Index].Right.Empty();

			RoomsData[Index].Forward.Append(RoomAdjacent[0]);
			RoomsData[Index].Back.Append(RoomAdjacent[1]);
			RoomsData[Index].Left.Append(RoomAdjacent[2]);
			RoomsData[Index].Right.Append(RoomAdjacent[3]);
		}
		else
		{
			CurrentRoom.Forward.Append(RoomAdjacent[0]);
			CurrentRoom.Back.Append(RoomAdjacent[1]);
			CurrentRoom.Left.Append(RoomAdjacent[2]);
			CurrentRoom.Right.Append(RoomAdjacent[3]);
			RoomsData.Add(CurrentRoom);
		}
		
	}
	


	for (FRoomData& Room : RoomsData)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedPtr<FJsonObject> JsonVariant = MakeShared<FJsonObject>();
		FJsonObjectConverter::UStructToJsonObject(FRoomData::StaticStruct(), &Room, JsonVariant.ToSharedRef(), 0, 0);
		JsonArray.Add(MakeShared<FJsonValueObject>(JsonVariant));
		JsonObject->SetArrayField(Room.RoomClass->GetName(), JsonArray);
	}

	//create json valid text
	FString OutputString;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);


	//save json to file
	FString FilePath = FPaths::ProjectSavedDir() / TEXT("MyData.json");

	FFileHelper::SaveStringToFile(OutputString, *FilePath);
}

