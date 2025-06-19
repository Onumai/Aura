// Copyright Patrick Haubner


#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent"); //Already a variable in AIController class
	check(Blackboard);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTree>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);
}
