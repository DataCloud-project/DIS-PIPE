import json

database_host = input("Enter the postgress database host: ")
port = input("Enter the port number: ")
user = input("Enter the username: ")
password = input("Enter the password: ")

# Construct the database configuration dictionary
database_config = {
    "database": {
        "populate" : "true",
        "database_host": database_host,
        "port": port,
        "user": user,
        "password": password
    }
}

# Path to the JSON file
json_file_path = "database_config.json"

# Write the database configuration to the JSON file
with open(json_file_path, "w") as json_file:
    json.dump(database_config, json_file, indent=4)

print("JSON file populated successfully!")