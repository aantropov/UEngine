-- Mesh Export Utility
-- the part of AnginEngine
-- by aka, 2005


struct t_material (  dr, dg, db, da, spr, spg, spb, ar, ag, ab,shin,   two_sided )
struct t_mesh ( name, vert_buf, face_buf, material, unique_verts, data_size )
struct t_vertex ( x, y, z, nx, ny, nz, u, v )
struct t_face ( v0, v1, v2 )

function transformVertsToEngine &vert_buf =
(
	for i=1 to vert_buf.count do
	(
	-- coords
		local y = vert_buf[i].y
		vert_buf[i].y = vert_buf[i].z
		vert_buf[i].z = y
	-- normals
		y = vert_buf[i].ny
		vert_buf[i].ny = vert_buf[i].nz
		vert_buf[i].nz = y
	)
)

function convertObjectToMesh c_obj &dest_mesh =
(
	if superClassOf c_obj != GeometryClass or not isValidNode c_obj  do
	( return false )

	dest_mesh.name =  c_obj.name
	
	local cmesh = snapshotAsMesh c_obj
	
	if cmesh.numverts==0 or cmesh.numfaces==0 do
	(
		messageBox "Invalid geometry." title:"Error"
		return false
	)
	
	if cmesh.numtverts>0 and cmesh.numtverts<cmesh.numverts do
	( messageBox "Non zero number of UV coords is less then number of vertices.\n"+
		(cmesh.numtverts as string)+" < "+(cmesh.numverts as string)  title:"Warning" )
	
	dest_mesh.unique_verts = cmesh.numverts;
	
	-- process material
	if c_obj.material!=undefined and classof c_obj.material==Standardmaterial then
	(
		dest_mesh.material.two_sided = 0
		if c_obj.material.twoSided do
		( dest_mesh.material.two_sided = 1 )
		dest_mesh.material.dr = c_obj.material.diffuse.r
		dest_mesh.material.dg = c_obj.material.diffuse.g
		dest_mesh.material.db = c_obj.material.diffuse.b
		dest_mesh.material.da = 255.0*(c_obj.material.opacity/100.0)
	
		dest_mesh.material.spr = c_obj.material.specular.r
		dest_mesh.material.spg = c_obj.material.specular.g
		dest_mesh.material.spb = c_obj.material.specular.b
				
		dest_mesh.material.ar = c_obj.material.ambient.r
		dest_mesh.material.ag = c_obj.material.ambient.g
		dest_mesh.material.ab = c_obj.material.ambient.b
	
		dest_mesh.material.shin = c_obj.material.glossiness
		
	)
	else
		( dest_mesh.material = t_material 200 200 200 255 0 )
	
	-- detect mirror
	local dsign = c_obj.transform.determinantsign
	
	-- process faces
	local ib = #(1, 3, 2)
	if dsign<0 do ( ib = #(1, 2, 3) )
	local cface = #(0, 0, 0)
	if cmesh.numtverts!=0 then
	(
		for i=1 to cmesh.numfaces do
		(
			local f = getTVFace cmesh i
			cface = #(f.x-1, f.y-1, f.z-1)
			dest_mesh.face_buf[i] = ( t_face cface[ib[1]] cface[ib[2]] cface[ib[3]] )
		)
	)
	else
	(
		for i=1 to cmesh.numfaces do
		(
			local f = getFace cmesh i
			cface = #(f.x-1, f.y-1, f.z-1)
			dest_mesh.face_buf[i] = ( t_face cface[ib[1]] cface[ib[2]] cface[ib[3]] )
		)
	)
	
	-- process vertices
	if cmesh.numtverts>cmesh.numverts then
	(
		-- build vertex index conversion buffer
		local temp_buf = #()
		for i=1 to cmesh.numfaces do
		(
			local f_uv = getTVFace cmesh i
			local f_vert = getFace cmesh i
			temp_buf[f_uv.x] = f_vert.x
			temp_buf[f_uv.y] = f_vert.y
			temp_buf[f_uv.z] = f_vert.z
		)
		-- store verts
		for i=1 to cmesh.numtverts do
		(
			local v = getVert cmesh temp_buf[i]
			local n = getNormal cmesh temp_buf[i]
			local uv = getTVert cmesh i
			
			dest_mesh.vert_buf[i] = ( t_vertex v.x v.y v.z n.x n.y n.z uv.x (1.0-uv.y) )
		)
	)
	else
	(
		for i=1 to cmesh.numverts do
		(
			local v = getVert cmesh i
			local n = getNormal cmesh i
			if cmesh.numtverts==0 then
			( dest_mesh.vert_buf[i] = ( t_vertex v.x v.y v.z n.x n.y n.z 0.0 0.0 ) )
			else
			(
				local uv = getTVert cmesh i
				dest_mesh.vert_buf[i] = ( t_vertex v.x v.y v.z n.x n.y n.z uv.x (1.0-uv.y) )
			)
		)
	)
	
	-- transform verts to directx coords system
	transformVertsToEngine dest_mesh.vert_buf
	
	-- compute mesh data size in bytes
	dest_mesh.data_size = dest_mesh.vert_buf.count*32 + dest_mesh.face_buf.count*6 +  11/*material*/ + dest_mesh.name.count+1 + 4/*header*/
	
	return true
)


function writeMesh &c_mesh &file_out = 
(
	-- write mesh data size
	writeLong file_out c_mesh.data_size
	-- write mesh header
	writeLong file_out c_mesh.vert_buf.count
	writeLong file_out c_mesh.face_buf.count
	-- write mesh name
	writeString file_out c_mesh.name
	-- write mesh material
	writeByte file_out c_mesh.material.dr
	writeByte file_out c_mesh.material.dg
	writeByte file_out c_mesh.material.db
	writeByte file_out c_mesh.material.da


	writeByte file_out c_mesh.material.two_sided
	
	writeByte file_out c_mesh.material.ar
	writeByte file_out c_mesh.material.ag
	writeByte file_out c_mesh.material.ab
	
	writeByte file_out c_mesh.material.spr
	writeByte file_out c_mesh.material.spg
	writeByte file_out c_mesh.material.spb
	
	writeByte file_out c_mesh.material.shin
		
	-- write mesh faces
	for i=1 to c_mesh.face_buf.count do
	(
		writeShort file_out c_mesh.face_buf[i].v0
		writeShort file_out c_mesh.face_buf[i].v1
		writeShort file_out c_mesh.face_buf[i].v2
	)
	-- write mesh verts
	for i=1 to c_mesh.vert_buf.count do
	(
		writeFloat file_out c_mesh.vert_buf[i].x
		writeFloat file_out c_mesh.vert_buf[i].y
		writeFloat file_out c_mesh.vert_buf[i].z
		writeFloat file_out c_mesh.vert_buf[i].nx
		writeFloat file_out c_mesh.vert_buf[i].ny
		writeFloat file_out c_mesh.vert_buf[i].nz
		writeFloat file_out c_mesh.vert_buf[i].u
		writeFloat file_out c_mesh.vert_buf[i].v
	)
)

function exportObject sel_index =
(
	local c_mesh = (t_mesh  "object" #() #() (t_material 0 0 0 0 0) 0 0)
	if not (convertObjectToMesh selection[sel_index] &c_mesh) do
	( return false )
	
	-- select file
	file_name = GetSaveFileName \
	caption:"Select File To Export" \
	filename:"Untitled" \
	types:"UEngine Polygonal Model (*.upm)|*.upm|"
	
	-- open file
	if file_name == undefined do
	( return true )
	file_out = fopen file_name "wb"
	if file_out == undefined do
	( return false )
	
	-- write header
	writeLong file_out 111 -- id - "AKA"
	
	writeLong file_out 1 -- mesh count
	
	writeMesh c_mesh file_out
	-- close file
	fclose file_out
	
	-- show result
	messageBox ("Name:\t\t" + c_mesh.name + "                                                        " +
				"\nVerts:\t\t" + (c_mesh.vert_buf.count as string) + " ( unique: " + (c_mesh.unique_verts as string) + " )" + 
				"\nFaces:\t\t" + (c_mesh.face_buf.count as string) +
				"\nMaterial:\t [ " + (c_mesh.material.dr as string) + ", " + (c_mesh.material.dg as string) + ", " + (c_mesh.material.db as string) +
					", " +(c_mesh.material.da as string) + " ] \ttwo sided:" + (c_mesh.material.two_sided as string) + 
				"\n\nData size:\t" + (c_mesh.data_size as string) + " bytes\n"
				) title:"Result                                  "
	return true
)

function exportAll n =
(
	-- select file
	file_name = GetSaveFileName \
	caption:"Select File To Export" \
	filename:"Untitled" \
	types:"UEngine Polygonal Model (*.upm)|*.upm|"
	
	-- open file
	if file_name == undefined do
	( return true )
	file_out = fopen file_name "wb"
	if file_out == undefined do
	( return false )
	
	-- write header
	writeLong file_out 111 -- id - "aka"
	-- write type header
	writeLong file_out selection.count -- mesh count
	for m=1 to selection.count do
	(
		-- convert object
		local c_mesh = (t_mesh  "object" #() #() (t_material 0 0 0 0 0) 0 0)
		if not (convertObjectToMesh selection[m] &c_mesh) do
		( return false )
		-- write mesh
		writeMesh c_mesh file_out
		-- show result
		
	)
		messageBox ("Exported!\n"
				) title:"Result                                  "		
	-- close file
	fclose file_out
	
	return true
)

rollout UtilDialog "UEngine Mesh Export Utility" width:300 height:300
(
	label uiLabelInfo "Exports scene objects as .UPM\n(UEngine Polygonal Model) file." align:#left width:290 height:40
	label uiLabelList "Select object to export" align:#left
	listbox uiSelList /*items:#("1","2","3")*/ height:10
	button uiBtnExport "Export Selected"  width:100 height:25
	button uiBtnExportAll "Export All"  width:100 height:25
	button uiBtnClose "Close"  width:100 height:25
	
	on UtilDialog open do
	(
		local i
		local mesh_array=#()-- = selection as array
		for i=1 to selection.count do
			mesh_array[i] = selection[i].name
		
		if selection.count==0 do
		(
			uiBtnExport.enabled = false
			uiBtnExportAll.enabled = false
		)
		
		uiSelList.items = mesh_array
	)
	
	local export_index = 0
	
	on uiSelList doubleClicked export_index do
	(
		if not exportObject export_index then
			messageBox "Can't export object." title:"Error"
	)
	
	on uiBtnExport pressed do
	(
		if not exportObject uiSelList.selection then
			messageBox "Can't export object." title:"Error"
	)
	
	on uiBtnExportAll pressed do
	(
		if not exportAll n then
			messageBox "Can't export object." title:"Error"
	)
	
	on uiBtnClose pressed do
	(
		DestroyDialog UtilDialog
		-- end
	)
)

if UtilDialogFloater != undefined do
(
	closerolloutfloater UtilDialogFloater
)

CreateDialog  UtilDialog width:210 height:320
