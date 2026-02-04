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

}

// Called when the game starts or when spawned
void ADataCollector::BeginPlay()
{
	Super::BeginPlay();

	//create json structure in memory
	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();

	UWorld* World = GetWorld();

	TArray<FJsonTest> DataArray;

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[%hs]: found simple actor: %s"), __FUNCTION__, *ItActor->GetName());

		FVector Start = ItActor->GetActorLocation();
		float Distance = 300.0f;

		FJsonTest Test;

		Test.CurrentBP = ItActor->GetClass();

		TArray<FVector> Directions;
		Directions.SetNum(4);

		Directions[0] = ItActor->GetActorForwardVector();
		Directions[1] = -ItActor->GetActorForwardVector();
		Directions[2] = ItActor->GetActorRightVector();
		Directions[3] = -ItActor->GetActorRightVector();

		for (int i = 0; i < 4; i++)
		{
			Directions[i].Normalize();

			FVector End = Start + (Directions[i] * Distance);

			FHitResult Hit;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(*ItActor);

			bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

			if (bHit)
			{
				UE_LOG(LogTemp, Log, TEXT("Hit %s at ^s"), *Hit.GetActor()->GetName(), *Hit.ImpactPoint.ToString());
				DrawDebugLine(World, Start, End, FColor::Green, true, 100.0f, 0, 1.5f);
				Test.Forward.Add(Hit.GetActor()->GetClass());
				DataArray.Add(Test);
			}
			else
				DrawDebugLine(World, Start, End, FColor::Red, true, 100.0f, 0, 1.5f);

		}
	}
	
	for (FJsonTest& Item : DataArray)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedPtr<FJsonObject> JsonVariant = MakeShared<FJsonObject>();
		FJsonObjectConverter::UStructToJsonObject(FJsonTest::StaticStruct(), &Item, JsonVariant.ToSharedRef(), 0, 0);
		JsonArray.Add(MakeShared<FJsonValueObject>(JsonVariant));
		RootObject->SetArrayField(Item.CurrentBP->GetName(), JsonArray);
	}

	//create json valid text
	FString OutputString;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);


	//save json to file
	FString FilePath = FPaths::ProjectSavedDir() / TEXT("MyData.json");

	FFileHelper::SaveStringToFile(OutputString, *FilePath);
}

void ADataCollector::Temp()
{
	UWorld* World = GetWorld();

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[%hs]: found simple actor: %s"), __FUNCTION__, *ItActor->GetName());

		FVector Start = ItActor->GetActorLocation();
		float Distance = 300.0f;

		TArray<FVector> Directions;
		Directions.SetNum(4);

		Directions[0] = ItActor->GetActorForwardVector();
		Directions[1] = -ItActor->GetActorForwardVector();
		Directions[2] = ItActor->GetActorRightVector();
		Directions[3] = -ItActor->GetActorRightVector();

		for (int i = 0; i < 4; i++)
		{
			Directions[i].Normalize();

			FVector End = Start + (Directions[i] * Distance);

			FHitResult Hit;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(*ItActor);

			bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

			if (bHit)
			{
				UE_LOG(LogTemp, Log, TEXT("Hit %s at ^s"), *Hit.GetActor()->GetName(), *Hit.ImpactPoint.ToString());
				DrawDebugLine(World, Start, End, FColor::Green, true, 100.0f, 0, 1.5f);
			}
			else
				DrawDebugLine(World, Start, End, FColor::Red, true, 100.0f, 0, 1.5f);

		}

	}
}