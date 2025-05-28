# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Carpetas
SRC_DIR = .
OBJ_DIR = obj
#OUT_DIR = out
BIN = DvsGestureParser

# Archivos fuente y objeto
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Regla principal
all: dirs $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Crear directorios necesarios
dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUT_DIR)

# Limpieza
clean:
	rm -rf $(OBJ_DIR) $(BIN) $(OUT_DIR)


