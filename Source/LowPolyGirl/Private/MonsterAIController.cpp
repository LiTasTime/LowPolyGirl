#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "LowPolyGirl/LowPolyGirlCharacter.h"

AMonsterAIController::AMonsterAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsPlayerLooking = false;
}

void AMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    // Получаем ссылку на игрока и его камеру
    PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        ALowPolyGirlCharacter* LowPolyGirl = Cast<ALowPolyGirlCharacter>(PlayerCharacter);
        if (LowPolyGirl)
        {
            PlayerCamera = LowPolyGirl->FollowCamera;
        }
    }

    // Начинаем движение к игроку
    MoveToPlayer();
}

void AMonsterAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Проверяем, смотрит ли игрок на монстра
    CheckIfPlayerIsLooking();

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
        UCharacterMovementComponent* MovementComponent = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>();

        // Останавливаем или возобновляем движение в зависимости от состояния взгляда
        if (bIsPlayerLooking)
        {
            MovementComponent->DisableMovement();  // Останавливаем движение
        }
        else
        {
            MovementComponent->SetMovementMode(MOVE_Walking);  // Возобновляем движение
        }
    }
}

void AMonsterAIController::CheckIfPlayerIsLooking()
{
    if (!PlayerCamera || !GetPawn()) return;

    FVector CameraLocation = PlayerCamera->GetComponentLocation();
    FVector CameraForward = PlayerCamera->GetForwardVector();
    FVector MonsterLocation = GetPawn()->GetActorLocation();

    // Рассчитываем направление от камеры до монстра
    FVector ToMonster = (MonsterLocation - CameraLocation).GetSafeNormal();
    float DotProduct = FVector::DotProduct(CameraForward, ToMonster);

    // Проверка, смотрит ли игрок на монстра
    bool bIsFacingMonster = (DotProduct > AngleThreshold);

    // Используем Sweep (вместо LineTrace), чтобы учесть объекты, которые могут быть в пути
    TArray<FHitResult> HitResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlayerCharacter);  // Игнорируем игрока
    Params.AddIgnoredActor(GetPawn());  // Игнорируем текущего монстра

    // Игнорируем другие объекты, кроме монстра и игрока
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterAIController::StaticClass(), FoundActors);
    Params.AddIgnoredActors(FoundActors);

    FVector EndLocation = CameraLocation + (ToMonster * TraceDistance);

    // Используем Sweep, который учтет физические столкновения на пути
    bool bHit = GetWorld()->SweepMultiByChannel(HitResults, CameraLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), Params);

    // Проверка видимости монстра
    bool bMonsterVisible = bIsFacingMonster && HitResults.Num() == 0 && GetPawn()->WasRecentlyRendered();

    // Обновляем состояние bIsPlayerLooking
    bIsPlayerLooking = bMonsterVisible;

    // Для отладки
    /*if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("DotProduct: %f"), DotProduct));
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Player Looking: %s"), bIsPlayerLooking ? TEXT("True") : TEXT("False")));
    }

    // Отображаем линию трассировки
    DrawDebugLine(GetWorld(), CameraLocation, MonsterLocation, FColor::Green, false, 0.1f, 0, 2.0f);*/
}

void AMonsterAIController::MoveToPlayer()
{
    if (PlayerCharacter && GetPawn())
    {
        MoveToActor(PlayerCharacter);
        GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AMonsterAIController::MoveToPlayer, 0.2f, false);
    }
}