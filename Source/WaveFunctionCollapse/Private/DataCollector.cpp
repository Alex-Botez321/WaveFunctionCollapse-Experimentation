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

	UWorld* World = GetWorld();

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		FVector Start = ItActor->GetActorLocation();

		FJSonCellData CurrentCell;
		CurrentCell.CellClass = ItActor->GetClass();
		CurrentCell.Weight = ItActor->SpawnWeight;

		TArray<TArray<TSubclassOf<ARoomBase>>> AdjacentCell;
		AdjacentCell.SetNum(DirectionCount);
		int32 Index = CellsData.Find(CurrentCell);
		if (Index != INDEX_NONE)
		{
			AdjacentCell[0].Append(CellsData[Index].Forward);
			AdjacentCell[1].Append(CellsData[Index].Back);
			AdjacentCell[2].Append(CellsData[Index].Left);
			AdjacentCell[3].Append(CellsData[Index].Right);
		}

		TArray<FVector> Direction;
		Direction.SetNum(DirectionCount);

		Direction[0] = ItActor->GetActorForwardVector();
		Direction[1] = -ItActor->GetActorForwardVector();
		Direction[2] = ItActor->GetActorRightVector();
		Direction[3] = -ItActor->GetActorRightVector();
		Direction[4] = ItActor->GetActorUpVector();
		Direction[5] = -ItActor->GetActorUpVector();

		for (int i = 0; i < DirectionCount; i++)
		{
			Direction[i].Normalize();

			FVector End = Start + (Direction[i] * Distance);

			FHitResult Hit;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(*ItActor);

			bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

			if (bHit)
			{

				if (!Hit.GetActor()->GetClass()->IsChildOf(ARoomBase::StaticClass()))
				{
					UE_LOG(LogTemp, Warning, TEXT("Hit %s at ^s, not subclass of ARoomBase"), *Hit.GetActor()->GetName(), *Hit.ImpactPoint.ToString());
					continue;
				}

				TSubclassOf<ARoomBase> HitClass = Hit.GetActor()->GetClass();

				UE_LOG(LogTemp, Log, TEXT("Hit %s at ^s"), *Hit.GetActor()->GetName(), *Hit.ImpactPoint.ToString());
				DrawDebugLine(World, Start, End, FColor::Green, true, 100.0f, 0, 1.5f);

				if (!AdjacentCell[i].Contains(HitClass))
				{
					AdjacentCell[i].Add(HitClass);
				}
				continue;
			}

			DrawDebugLine(World, Start, End, FColor::Red, true, 100.0f, 0, 1.5f);

		}

		//add the data of the current cell to the data array.
		if (Index != INDEX_NONE)
		{
			//To Do: find better way to work around ue5 2d array limitation to remove the need to constantly empty arrays.
			CellsData[Index].Forward.Empty();
			CellsData[Index].Back.Empty();
			CellsData[Index].Left.Empty();
			CellsData[Index].Right.Empty();
			CellsData[Index].Up.Empty();
			CellsData[Index].Down.Empty();

			CellsData[Index].Forward.Append(AdjacentCell[0]);
			CellsData[Index].Back.Append(AdjacentCell[1]);
			CellsData[Index].Left.Append(AdjacentCell[2]);
			CellsData[Index].Right.Append(AdjacentCell[3]);
			CellsData[Index].Up.Append(AdjacentCell[4]);
			CellsData[Index].Down.Append(AdjacentCell[5]);
		}
		else
		{
			CurrentCell.Forward.Append(AdjacentCell[0]);
			CurrentCell.Back.Append(AdjacentCell[1]);
			CurrentCell.Left.Append(AdjacentCell[2]);
			CurrentCell.Right.Append(AdjacentCell[3]);
			CurrentCell.Up.Append(AdjacentCell[4]);
			CurrentCell.Down.Append(AdjacentCell[5]);
			CellsData.Add(CurrentCell);
		}
		
	}

	//create json structure in memory
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	for (FJSonCellData& Room : CellsData)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedPtr<FJsonObject> JsonVariant = MakeShared<FJsonObject>();
		FJsonObjectConverter::UStructToJsonObject(FJSonCellData::StaticStruct(), &Room, JsonVariant.ToSharedRef(), 0, 0);
		JsonArray.Add(MakeShared<FJsonValueObject>(JsonVariant));
		JsonObject->SetArrayField(Room.CellClass->GetPathName(), JsonArray);
	}

	//create json valid text
	FString OutputString;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);


	//save json to file
	FString FilePath = FPaths::ProjectSavedDir() / TEXT("MyData.json");

	FFileHelper::SaveStringToFile(OutputString, *FilePath);
}

