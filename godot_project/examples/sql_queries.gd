extends Node

# SQLite module
const SQLite = preload("res://lib/gdsqlite.gdns");

func _ready():
	# Create gdsqlite instance
	var db = SQLite.new();
	
	# Open database
	if (!db.open_db("res://godot.sql")):
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
	result = db.query(query);
	
	# Fetch rows
	query = "SELECT * FROM users;";
	result = db.fetch_array(query);
	
	if (!result || result.size() <= 0):
		# Insert new row
		query = "INSERT INTO users (first_name, last_name, email) VALUES ('godot', 'engine', 'user@users.org');";
		result = db.query(query);
		
		if (!result):
			print("Cannot insert data!");
		else:
			print("Data inserted into table.");
	
	else:
		# Print rows
		for i in result:
			print(i);
	
	# Close database
	db.close();
