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

	TArray<FRoomData> RoomsArray;

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[%hs]: found simple actor: %s"), __FUNCTION__, *ItActor->GetName());

		FVector Start = ItActor->GetActorLocation();

		FRoomData Room;
		Room.RoomClass = ItActor->GetClass();

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
				Room.Forward.Add(Hit.GetActor()->GetClass());
				RoomsArray.Add(Room);
			}
			else
				DrawDebugLine(World, Start, End, FColor::Red, true, 100.0f, 0, 1.5f);

		}
	}
	
	for (FRoomData& Room : RoomsArray)
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

