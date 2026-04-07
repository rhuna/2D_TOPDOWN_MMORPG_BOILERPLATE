$ErrorActionPreference = "Stop"

$worldPath = "client/src/world/World.cpp"
$backupPath = "client/src/world/World_before_split_backup.cpp"
$drawPath = "client/src/world/World_Draw.cpp"
$choicePath = "client/src/world/World_Choice.cpp"

if (!(Test-Path $worldPath)) {
    throw "Could not find $worldPath"
}

$raw = Get-Content $worldPath -Raw
$lines = $raw -split "`r?`n"

function Find-FunctionStart([string]$pattern) {
    for ($i = 0; $i -lt $lines.Length; $i++) {
        if ($lines[$i] -match $pattern) {
            return $i
        }
    }
    throw "Could not find function start matching pattern: $pattern"
}

function Get-BlockRange([int]$startIndex) {
    $braceCount = 0
    $foundBrace = $false

    for ($i = $startIndex; $i -lt $lines.Length; $i++) {
        $line = $lines[$i].ToCharArray()
        foreach ($ch in $line) {
            if ($ch -eq '{') {
                $braceCount++
                $foundBrace = $true
            }
            elseif ($ch -eq '}') {
                $braceCount--
            }
        }

        if ($foundBrace -and $braceCount -eq 0) {
            return @{
                Start = $startIndex
                End   = $i
            }
        }
    }

    throw "Could not find matching brace block for line index $startIndex"
}

function TryGet-FunctionBlock([string]$pattern) {
    try {
        return Get-FunctionBlock $pattern
    }
    catch {
        Write-Host "Skipping missing function pattern: $pattern" -ForegroundColor Yellow
        return $null
    }
}

function Get-CombinedBlocks([string[]]$patterns) {
    $parts = @()
    foreach ($p in $patterns) {
        $block = TryGet-FunctionBlock $p
        if ($null -ne $block -and $block.Trim().Length -gt 0) {
            $parts += $block
            $parts += ""
        }
    }
    return ($parts -join "`r`n").TrimEnd() + "`r`n"
}

$drawStart = Find-FunctionStart '^\s*void\s+World::Draw\s*\(\)\s+const'
$drawPrefix = ($lines[0..($drawStart - 1)] -join "`r`n").TrimEnd() + "`r`n`r`n"

$drawFunctions = @(
    '^\s*void\s+World::Draw\s*\(\)\s+const',
    '^\s*void\s+World::DrawMap\s*\(\)\s+const',
    '^\s*void\s+World::DrawHud\s*\(\)\s+const',
    '^\s*void\s+World::DrawMinimap\s*\(\)\s+const',
    '^\s*void\s+World::DrawWorldMapOverlay\s*\(\)\s+const',
    '^\s*void\s+World::DrawPlayer\s*\(\)\s+const',
    '^\s*void\s+World::DrawNpc\s*\(const\s+Npc&\s+npc\)\s+const',
    '^\s*void\s+World::DrawEnemy\s*\(const\s+Enemy&\s+enemy\)\s+const',
    '^\s*void\s+World::DrawRemotePlayer\s*\(const\s+RemoteActor\s*&remote\)\s+const',
    '^\s*void\s+World::DrawAnimatedActor\s*\(const\s+Actor&\s+actor,\s*const\s+DirectionalSprite&\s+spriteSet,\s*Color\s+tint\)\s+const',
    '^\s*void\s+World::DrawPickupSprite\s*\(Vector2\s+position,\s*Rectangle\s+source,\s*Color\s+tint\)\s+const'
)

$choiceFunctions = @(
    '^\s*void\s+World::OpenChoiceUi\s*\(const\s+std::string\s*&questId\)',
    '^\s*void\s+World::CloseChoiceUi\s*\(\)',
    '^\s*void\s+World::UpdateChoiceUi\s*\(\)',
    '^\s*void\s+World::DrawChoiceUi\s*\(\)\s+const'
)

$coreFunctions = @(
    '^\s*void\s+World::UpdateRemotePlayers\s*\(',
    '^\s*void\s+World::LoadAssets\s*\(',
    '^\s*void\s+World::SetupAtlas\s*\(',
    '^\s*void\s+World::UpdatePlayer\s*\(',
    '^\s*void\s+World::UpdateEnemies\s*\(',
    '^\s*void\s+World::HandleCombat\s*\(',
    '^\s*void\s+World::HandleInteraction\s*\(',
    '^\s*void\s+World::HandleDrops\s*\(',
    '^\s*Vector2\s+World::GetPlayerPosition\s*\(',
    '^\s*Vector2\s+World::GetWorldPixelSize\s*\(',
    '^\s*void\s+World::UpdateCamera\s*\(',
    '^\s*void\s+World::TryPickup\s*\(',
    '^\s*bool\s+World::IsWall\s*\(',
    '^\s*bool\s+World::IsClose\s*\(',
    '^\s*Rectangle\s+World::Cell\s*\(',
    '^\s*int\s+World::DirectionIndex\s*\(',
    '^\s*Direction\s+World::DirectionFromVector\s*\(',
    '^\s*const\s+AnimationClip&\s+World::SelectClip\s*\(',
    '^\s*const\s+AtlasFrame&\s+World::ResolveFrame\s*\(',
    '^\s*int\s+World::TileVariantIndex\s*\(',
    '^\s*void\s+World::HandleBuildingTransitions\s*\('
)

$standardIncludeBlock = @'
#include "world/World.h"
#include "world/Data.h"
#include "net/NetworkClient.h"
#include "game/QuestSystem.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include "raylib.h"

'@

if (!(Test-Path $backupPath)) {
    Copy-Item $worldPath $backupPath
}

$drawContent = $standardIncludeBlock + (Get-CombinedBlocks $drawFunctions)
$choiceContent = $standardIncludeBlock + (Get-CombinedBlocks $choiceFunctions)
$coreContent = $drawPrefix + (Get-CombinedBlocks $coreFunctions)

Set-Content -Path $drawPath -Value $drawContent -Encoding UTF8
Set-Content -Path $choicePath -Value $choiceContent -Encoding UTF8
Set-Content -Path $worldPath -Value $coreContent -Encoding UTF8

Write-Host "Split complete:"
Write-Host "  $worldPath"
Write-Host "  $drawPath"
Write-Host "  $choicePath"
Write-Host "Backup saved to:"
Write-Host "  $backupPath"