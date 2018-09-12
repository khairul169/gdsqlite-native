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
	if (not db.open_db("user://player_stats.sql")):
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
	if (db and db.loaded()):
		# Close database
		db.close();

func set_highscore(score):
	if (not db or not db.loaded()):
		return;
	
	# Update highscore
	highscore = score;
	
	# Execute sql syntax
	if (row_id > 0):
		db.query(str("UPDATE highscore SET score='", highscore, "' WHERE id='", row_id, "';"));
	else:
		db.query(str("INSERT INTO highscore (score) VALUES ('", highscore, "');"));
		row_id = db.last_insert_rowid();

func get_highscore():
	if (not db or not db.loaded()):
		return;
	
	# Retrieve highscore from database
	var rows = db.fetch_array("SELECT score FROM highscore WHERE id='" + str(row_id) + "' LIMIT 1;");
	if (rows and not rows.empty()):
		highscore = rows[0]['score'];
	
	# Return the highscore
	return highscore;
