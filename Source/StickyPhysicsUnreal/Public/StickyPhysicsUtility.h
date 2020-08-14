// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StickyTriangle.h"

/**
 * 
 */
struct STICKYPHYSICSUNREAL_API FStickyPhysicsUtility
{
	static bool TraceForTriangle(FStickyTriangle& Result, const UWorld* World, const FVector& TraceStart, const FVector& TraceEnd, ECollisionChannel Channel);
};
