extends Node

func _ready():
	var query = "";
	var result = null;
	
	# Open database file
	if (SQLite.open_db("test.sql") != OK):
		print("Cannot open database!");
	
	# Create table
	query = "CREATE TABLE IF NOT EXISTS test (";
	query += "id integer PRIMARY KEY,";
	query += "first_name text NOT NULL,";
	query += "last_name text NOT NULL,";
	query += "email text NOT NULL";
	query += ");";
	result = SQLite.query(query);
	
	if (!result):
		print("Error: cannot create table.");
	
	# Fetch rows
	query = "SELECT * FROM test;";
	result = SQLite.fetch_array(query);
	
	if (result.size() <= 0):
		# Insert new row
		query = "INSERT INTO test (first_name, last_name, email) VALUES ('godot', 'engine', 'user@test.org');";
		result = SQLite.query(query);
		
		if (!result):
			print("Error: cannot insert data");
		else:
			print("Data inserted.");
	else:
		# Print fetched rows
		for i in result:
			print(i);
