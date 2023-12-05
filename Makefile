# Nome do programa
TARGET  = Cyberjewels

# Diretórios
SRCDIR  = ./src
INCDIR  = ./lib
OBJDIR  = ./obj
SAVEDIR = ./saves


# Arquivos .c e .o
CFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))

# Opções de compilação
CC = gcc
CFLAGS = -Wall -std=c99
LIBALLEGRO = -lallegro_image -lallegro_dialog -lallegro_primitives -lallegro_ttf \
			 -lallegro_font -lallegro_audio -lallegro_acodec -lallegro -lm

# Compilar e Linkar
all: folders $(TARGET) clean
	@echo "Done."

$(TARGET): $(OBJFILES)
	@echo "Building $@" 
	@$(CC) $(CFLAGS) $^ -o $@ $(LIBALLEGRO)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "Building $@" 
	@$(CC) $(CFLAGS) -c $^ -o $@

folders:
	@if [ ! -d "$(OBJDIR)" ]; then \
		mkdir -p $(OBJDIR); \
	fi

	@if [ ! -d "$(SAVEDIR)" ]; then \
		mkdir -p $(SAVEDIR); \
	fi

clean: 
	@if [ -d "$(OBJDIR)" ]; then \
		rm -rf $(OBJDIR); \
	fi

purge: clean
	@rm -f $(TARGET)
	@if [ -d "$(SAVEDIR)" ]; then \
		rm -rf $(SAVEDIR); \
	fi

.PHONY: all purge clean
