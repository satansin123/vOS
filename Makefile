# Project configuration
PROJECT_NAME = CppProject
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

# Default target
.PHONY: all
all: build

# Configure the project
.PHONY: configure
configure:
	@echo "Configuring project..."
	@cmake -B $(BUILD_DIR) -S . -G Ninja

# Build the project
.PHONY: build
build: configure
	@echo "Building project..."
	@cmake --build $(BUILD_DIR)

# Run the application
.PHONY: start run
start run: build
	@echo "Running application..."
	@./$(BIN_DIR)/$(PROJECT_NAME) local

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)

# Docker targets
.PHONY: docker-build
docker-build:
	@echo "Building Docker image..."
	@docker build -t cpp-project .

.PHONY: docker-start docker-run
docker-start docker-run: docker-build
	@echo "Running in Docker container..."
	@docker run -it --rm cpp-project

# Docker development container
.PHONY: docker-dev
docker-dev:
	@echo "Starting development container..."
	@docker-compose up -d dev
	@docker-compose exec dev bash

.PHONY: docker-dev-stop
docker-dev-stop:
	@echo "Stopping development container..."
	@docker-compose down

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  build          - Configure and build the project"
	@echo "  start/run      - Build and run the application locally"
	@echo "  clean          - Clean build artifacts"
	@echo "  docker-build   - Build Docker image"
	@echo "  docker-start   - Run application in Docker container"
	@echo "  docker-dev     - Start development container (interactive)"
	@echo "  docker-dev-stop- Stop development container"
	@echo "  help           - Show this help message"
