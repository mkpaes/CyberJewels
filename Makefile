# Nome do programa
TARGET = Cyberjewels

SRCDIR = ./src
INCDIR = ./lib

SAVEDIR = saves

# Arquivos .c e .o
CFILES = $(foreach D, $(SRCDIR), $(wildcard $(D)/*.c))
OBJFILES = $(patsubst %.c, %.o, $(CFILES))

# Opções de compilação
CC = gcc
FLAGS = -Wall -std=c99
ALLEGRO = 	-lallegro_image -lallegro_dialog -lallegro_primitives -lallegro_ttf \
			-lallegro_font -lallegro_audio -lallegro_acodec -lallegro -lm

# Compilar e Linkar
all: $(TARGET) $(SAVEDIR)

$(TARGET): $(OBJFILES)
	$(CC) -o $@ $^ $(FLAGS) $(ALLEGRO)

.%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $^

$(SAVEDIR):
	@if [ ! -d "$(SAVEDIR)" ]; then \
		mkdir -p $(SAVEDIR); \
	fi

clean: 
	@rm -f $(OBJFILES)

purge: clean
	@rm -f $(TARGET)
	@if [ -d "$(SAVEDIR)" ]; then \
		rm -rf $(SAVEDIR); \
	fi

.PHONY: all purge clean
