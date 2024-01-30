#!/bin/bash

# Path to your .env file
ENV_FILE=".env"

# Check if the .env file exists
if [ -f "$ENV_FILE" ]; then
  # Read each line of the .env file and set as environment variable
  while IFS= read -r line; do
    export "$line"
  done < "$ENV_FILE"

  echo "Environment variables set from $ENV_FILE"
else
  echo "Error: $ENV_FILE not found"
fi
