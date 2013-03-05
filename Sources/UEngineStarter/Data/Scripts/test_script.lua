local velocity;
local border;
local gameObject;
local tr;

function Init()
	gameObject = GetGameObject();
	tr = GetTransform(gameObject);
	velocity = {x = 0.001, y = 0.0001, z = 0.005, w = 0.0};
	border = {x = 15.0, y = 5.0, z = 13.0, w = 0.0}

	border = SummVectors(tr.pos, border);
	return 1;
end

function Release()
--	MessageBox("Release");
	return 1;
end

function Update(delta)
	if tr.pos.x > border.x or tr.pos.x < -1.0 * border.x then
		velocity.x = velocity.x * -1.0;
	end

	if tr.pos.z > border.z or tr.pos.z < -1.0 * border.z then
		velocity.z = velocity.z * -1.0;
	end

	if tr.pos.y > border.y or tr.pos.y < 0 then
		velocity.y = velocity.y * -1.0;
	end

	tr.pos = SummVectors(tr.pos, MultVectorScalar(velocity, delta));

	SetTransform(gameObject, tr);
	return 1;
end

function MultVectorScalar(vec, delta)
	return {x = vec.x * delta, y = vec.y * delta, z = vec.z * delta, w = vec.w * delta};
end

function SummVectors(a, b)
	return {x = a.x + b.x, y = a.y + b.y, z = a.z + b.z, w = a.w + b.w};
end