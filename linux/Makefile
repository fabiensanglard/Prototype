CC_FILES := \
../src/AnimGenerator.cpp \
../src/Beam.cpp \
../src/Boss.cpp \
../src/BrainBoss.cpp \
../src/BrainCell.cpp \
../src/BreakScenery.cpp \
../src/Bug.cpp \
../src/Bullet.cpp \
../src/Button.cpp \
../src/CheckPoint.cpp \
../src/Debris.cpp \
../src/Demo.cpp \
../src/Editor_Anims.cpp \
../src/Editor.cpp \
../src/Editor_EntityProperties.cpp \
../src/Engine.cpp \
../src/Engine_Generators.cpp \
../src/Entity.cpp \
../src/Explosion.cpp \
../src/Fighter.cpp \
../src/FileIO.cpp \
../src/firebug.cpp \
../src/Fire.cpp \
../src/Font.cpp \
../src/FrontEnd.cpp \
../src/Game.cpp \
../src/Generator.cpp \
../src/Geo2D.cpp \
../src/Geo2D_Draw.cpp \
../src/HUD.cpp \
../src/LadyBird.cpp \
../src/Log.cpp \
../src/Map.cpp \
../src/MidBoss.cpp \
../src/Orb.cpp \
../src/Player.cpp \
../src/PostEffects.cpp \
../src/PowerDroid.cpp \
../src/PowerUp.cpp \
../src/Profiler.cpp \
../src/ProtoType.cpp \
../src/RandExplosion.cpp \
../src/Rhino.cpp \
../src/Rocket.cpp \
../src/Serializer.cpp \
../src/Serpent.cpp \
../src/SolarFlare.cpp \
../src/Sound.cpp \
../src/SpaceTrash.cpp \
../src/Sperm.cpp \
../src/Sprite.cpp \
../src/SpriteEffect.cpp \
../src/SpriteSheet.cpp \
../src/StarBurst.cpp \
../src/stdafx.cpp \
../src/SuperTurret.cpp \
../src/Timer.cpp \
../src/TriggerSlowDown.cpp \
../src/TriggerSpeedUp.cpp \
../src/Turret.cpp \
../src/UTIL_bitop.cpp \
../src/UTIL_Fade.cpp \
../src/UTIL_Misc.cpp \
../src/UTIL_openGL.cpp \
../src/UTIL_SDL.cpp

O_FILES := $(CC_FILES:.cpp=.o)
X_FILES := ../bin/prototype.x

# requires devil
# http://downloads.sourceforge.net/openil/DevIL-1.7.8.tar.gz
# ./configure --prefix=$HOME/install --enable-ILUT --enable-ILU

DEVIL_DIR ?= /usr

CC := g++
CFLAGS := -Wall -O2 -Wno-write-strings -Wno-unknown-pragmas -I../src
CFLAGS += -I$(DEVIL_DIR)/include
LD := g++
LFLAGS := -lSDL -lSDL_mixer -lGL -lGLEW -L$(DEVIL_DIR)/lib -lIL -lILU -lILUT

all: $(X_FILES)
$(X_FILES): $(O_FILES)
	$(LD) -o $@ $(O_FILES) $(LFLAGS)
%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	@rm -f $(O_FILES)
fclean: clean
	@rm -f $(X_FILES)
