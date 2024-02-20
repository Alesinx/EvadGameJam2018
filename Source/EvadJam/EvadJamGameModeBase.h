// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EvadJamGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class EVADJAM_API AEvadJamGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	class AGrid* Grid;
	
};
