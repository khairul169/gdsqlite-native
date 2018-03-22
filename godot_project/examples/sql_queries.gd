extends Node

func _ready():
	# Open database
	if (!SQLite.open_db("godot.sql")):
		print("Cannot open database.");
		return;
	
	var query = "";
	var result = null;
	
	# Create table
	query = "CREATE TABLE IF NOT EXISTS users (";
	query += "id integer PRIMARY KEY,";
	query += "first_name text NOT NULL,";
	query += "last_name text NOT NULL,";
	query += "email text NOT NULL";
	query += ");";
	result = SQLite.query(query);
	
	# Fetch rows
	query = "SELECT * FROM users;";
	result = SQLite.fetch_array(query);
	
	if (!result || result.size() <= 0):
		# Insert new row
		query = "INSERT INTO users (first_name, last_name, email) VALUES ('godot', 'engine', 'user@users.org');";
		result = SQLite.query(query);
		
		if (!result):
			print("Cannot insert data!");
		else:
			print("Data inserted into table.");
	
	else:
		# Print rows
		for i in result:
			print(i);
	
	# Close database
	SQLite.close();
