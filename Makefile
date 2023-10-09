# Nome do programa
EXEC = Cyberjewels

# Arquivos .o + main.o
OBJS =	./obj/board.o \
		./obj/fileop.o \
		./obj/allib.o	\
		./obj/main.o	

# Opções de compilação
CC = gcc
FLAGS = -Wall -std=c99
ALLEGRO = 	-lallegro_image -lallegro_dialog -lallegro_primitives -lallegro_ttf \
			-lallegro_font -lallegro_audio -lallegro_acodec -lallegro -lm

# Compilar e Linkar
all: objFolder $(EXEC) saveFolder

$(EXEC): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@ $(ALLEGRO)

./obj/%.o: ./lib/%.c
	$(CC) $(FLAGS) -c $^ -o $@

objFolder:
	@mkdir -p obj

saveFolder:
	@mkdir -p saves

clean: 
	@rm -rf ./obj/*o *~
	@rm -df obj 

purge: clean
	@rm -rf $(EXEC)
	@rm -drf saves

.PHONY: all purge clean
