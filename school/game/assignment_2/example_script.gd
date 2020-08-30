extends Node


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	print("Here is Path2D node ", get_node("."))

	print("Here is Path2D's parent ", get_node(".").get_parent())
	get_tree().change_scene("SceneTwo")
	print("Now we are in Scene Two!")
	print("Here is the first child of SceneTwo ", get_tree().get_root().get_child(0))

	# Array Function
	var array = ["Hello", "Dr.", "Girard", 70000]
	print(array[2]) # One.
	array[0] = "Hi" 
	print(array[0]) # 3.

	# For loop
	for item in array:
		print(item)

	# Dictonary
	var example_dic = {
		"power_level": "1234",
		"health": "1",
		"stamina": "-5",
	}

	# Range
	for i in range(len(example_dic)):
		print(i)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
