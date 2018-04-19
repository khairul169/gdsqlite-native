extends Node

# variables
var item_list = [];

func _ready():
	# Open item database
	SQLite.open_db("res://items.db");
	
	# Get item list from db
	var pots = SQLite.fetch_array("SELECT * FROM potion ORDER BY id ASC");
	if (not pots or pots.empty()):
		return;
	
	for pot in pots:
		# Create new item from database
		var item = {
			'id': pot['id'],
			'name': pot['name'],
			'price': pot['price'],
			'heals': pot['heals']
		};
		
		# Add to item list
		item_list.append(item);
	
	# Print all item
	for i in item_list:
		print("Item ", i.id, " (", i.name, ") $", i.price, " +", i.heals, "hp");
