extends Node

# SQLite module
const SQLite = preload("res://lib/gdsqlite.gdns");

# Variables
var db;
var highscore = 0;
var row_id = 0;

func _ready():
	# Create gdsqlite instance
	db = SQLite.new();
	
	# Open the database
	if (not db.open("user://player_stats.sql")):
		return;
	
	# Create table
	var query = "CREATE TABLE IF NOT EXISTS highscore (id INTEGER PRIMARY KEY, score INTEGER NOT NULL);";
	if (not db.query(query)):
		return;
	
	# Retrieve current highscore
	var rows = db.fetch_array("SELECT id, score FROM highscore LIMIT 1;");
	if (rows and not rows.empty()):
		row_id = rows[0]['id'];
		highscore = rows[0]['score'];
	
	# Test
	set_highscore(1000);
	set_highscore(2000);
	set_highscore(10000);
	set_highscore(50000);
	print("High score: ", get_highscore());

func _exit_tree():
	if (db):
		# Close database
		db.close();

func set_highscore(score):
	if (not db):
		return;
	
	# Update highscore
	highscore = score;
	
	# Execute sql syntax
	if (row_id > 0):
		var args = [highscore, row_id]
		db.query_with_args("UPDATE highscore SET score=? WHERE id=?;", args);
	else:
		var args = [row_id];
		db.query_with_args("INSERT INTO highscore (score) VALUES (?);", args);
		row_id = db.fetch_array("SELECT last_insert_rowid()", [])[0]['last_insert_rowid()'];

func get_highscore():
	if (not db):
		return;
	
	# Retrieve highscore from database
	var rows = db.fetch_array_with_args("SELECT score FROM highscore WHERE id=? LIMIT 1;", [row_id]);
	if (rows and not rows.empty()):
		highscore = rows[0]['score'];
	
	# Return the highscore
	return highscore;
