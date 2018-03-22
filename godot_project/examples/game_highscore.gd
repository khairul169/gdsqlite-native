extends Node

# Variables
var loaded = false;
var highscore = 0;
var row_id = 0;

func _ready():
	# Open database
	if (!SQLite.open_db("player_stats.sql")):
		return;
	else:
		loaded = true;
	
	# Create table
	var query = "CREATE TABLE IF NOT EXISTS highscore (id INTEGER PRIMARY KEY, score INTEGER NOT NULL);";
	if (!SQLite.query(query)):
		return;
	
	# Retrieve current highscore
	var rows = SQLite.fetch_array("SELECT id, score FROM highscore LIMIT 1;");
	if (rows && rows.size() > 0):
		row_id = rows[0]['id'];
		highscore = rows[0]['score'];
	
	# Test
	set_highscore(1000);
	set_highscore(2000);
	set_highscore(10000);
	set_highscore(50000);
	print("High score: ", get_highscore());

func _exit_tree():
	# Close database
	SQLite.close();

func set_highscore(score):
	if (!SQLite.loaded()):
		return;
	
	# Update highscore
	highscore = score;
	
	# Execute sql syntax
	if (row_id > 0):
		SQLite.query(str("UPDATE highscore SET score='", highscore, "' WHERE id='", row_id, "';"));
	else:
		SQLite.query(str("INSERT INTO highscore (score) VALUES ('", highscore, "');"));
		row_id = SQLite.last_insert_rowid();

func get_highscore():
	# Retrieve highscore from database
	var rows = SQLite.fetch_array("SELECT score FROM highscore WHERE id='" + str(row_id) + "' LIMIT 1;");
	if (rows && rows.size() > 0):
		highscore = rows[0]['score'];
	
	# Return the highscore
	return highscore;
