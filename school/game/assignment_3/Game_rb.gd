extends Node2D

const Fighter = preload("res://Fighter.tscn")
const Platform = preload("res://Platform.tscn")


# Called when the node enters the scene tree for the first time.
func _ready():
	get_node(".").add_child(Fighter.instance())
	get_node(".").add_child(Platform.instance())
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
