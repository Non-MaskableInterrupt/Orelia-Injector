#include <windows.h>
#include <winternl.h>
#include <intrin.h>
#include <pplwin.h>

#include <dependencies/vector/vector.hxx>
#include <dependencies/skcrypt/skcrypter.h>

#include <impl/std/std.hxx>
#include <impl/crt/crt.hxx>
#include <impl/pe/pe.hxx>
#include <impl/vmt/vmt.hxx>

#include <workspace/unreal/unreal.h>
#include <workspace/drawing/drawing.h>

#include <workspace/game/features/features.h>
#include <workspace/game/features/players/players.hxx>
#include <workspace/game/features/exploits/exploits.hxx>
#include <workspace/game/features/targeting/targeting.hxx>
#include <workspace/game/features/environment/environment.hxx>

#include <workspace/game/hooks/draw_transation.hxx>