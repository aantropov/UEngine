local test;
local gameObject;
local tr;

function set(a)	
	test = a;
	return 1;
end

function get(a)
	return test;
end

function Init()
	gameObject = GetGameObject();
	tr = GetTransform(gameObject);
	return 1;
end

function Release()
--	MessageBox("Release");
	return 1;
end

function Update(a)
	
	--tr.pos.x = tr.pos.y + 0.0001 * a;
	tr.rotation.y = tr.rotation.y  +  0.0001 * a;

	SetTransform(gameObject, tr);
	return 1;
end