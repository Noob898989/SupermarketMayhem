// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SupermarketMayhemCustomerData.h"
#include "SupermarketMayhemCustomerAIController.generated.h"

class ASupermarketMayhemCustomer;

UCLASS()
class SUPERMARKETMAYHEM_API ASupermarketMayhemCustomerAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASupermarketMayhemCustomerAIController();
	void SetBehaviorEnabled(bool bEnabled, bool bResetState);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	void ScheduleNextDestination(float Delay);
	void ChooseAndMoveToDestination();
	float GetRandomIdleDuration() const;
	float GetRandomShoppingDuration() const;
	void SetCustomerState(ESupermarketMayhemCustomerState NewState);

	TWeakObjectPtr<ASupermarketMayhemCustomer> Customer;
	FTimerHandle BehaviorTimerHandle;
	FVector LastDestination = FVector::ZeroVector;
	bool bBehaviorEnabled = false;
	bool bHasLoggedNavigationStatus = false;
};
