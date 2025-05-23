# Definir el compilador
CXX = g++

# OpenCV flags
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS = $(shell pkg-config --libs opencv4)

# Definir las banderas de compilación
CXXFLAGS = -Wall -g -Iinclude -fsanitize=address -std=c++11 $(OPENCV_CFLAGS)

# Definir las carpetas
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Crear la carpeta obj si no existe
$(shell mkdir -p $(OBJ_DIR))

# Definir los archivos fuente
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Definir los archivos objeto
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Definir el ejecutable
EXEC = SNN

# Regla por defecto
all: $(EXEC)

# Regla para enlazar el ejecutable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV_LIBS)

# Regla para compilar los archivos fuente
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(OBJS) $(EXEC)

# Regla para limpiar todos los archivos generados
distclean: clean
	rm -f *~

.PHONY: all clean distclean