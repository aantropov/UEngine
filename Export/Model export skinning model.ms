-- Mesh Export Utility
-- the part of UEngine
-- by aka, 2012

global UtilDialog

struct UEMaterial (dr, dg, db, da, spr, spg, spb, ar, ag, ab, shin, two_sided )
struct UEMesh (Name, VerticesBuffer, FaceBuffer, Material, UniqueVertices, DataSize)
struct UEVertex (x, y, z, nx, ny, nz, u, v, w)
struct UEFace (v0, v1, v2)
struct UEWeight (i0, i1, i2, i3, w0, w1, w2, w3)

function FindBoneID Name Bones =
(
    for i = 1 to Bones.count do
        if Bones[i].Name == Name then
            return (i - 1)
)

function FindAllBones &Bones =
(
    Bones = #()    
    for obj in objects do
        if ((classOf obj) == BoneGeometry) or ((classOf obj) == Biped_Object) /*or (matchPattern obj.Name  pattern:"Bone?")*/ then
            append Bones (obj)
)

function FindSkinModificator obj = 
(
    if (canConvertTo obj TriMeshGeometry) then
        for i = 1 to obj.modifiers.count do
            if obj.modifiers[i].Name == "Skin" then
            (                
                return obj.modifiers[i]
            )
    return undefined
)

--Thanks DOOM 3 Export Script
function ConvertMatrixToRightHanded Matrix =
(
    local oMat
    oMat = copy Matrix
    p = oMat.position
    r = oMat.rotation
    oMat.rotation = quat r.x r.z r.y r.w
    oMat.position = [p.x, p.z, p.y]
    return oMat
)

function ConvertVerticesToRightHanded &VerticesBuffer =
(
    for i = 1 to VerticesBuffer.count do
    (
        local y = VerticesBuffer[i].y
        VerticesBuffer[i].y = VerticesBuffer[i].z
        VerticesBuffer[i].z = y
    
        y = VerticesBuffer[i].ny
        VerticesBuffer[i].ny = VerticesBuffer[i].nz
        VerticesBuffer[i].nz = y
    )
)

function ConvertOkToFloat a =
(
	if a == Ok then
		return 0.0
    return a
)

struct temp_weight (weight, id)

function GetWeightData Vertex Mesh Bones  = 
(
    function CompareBones a b = 
    (
        if a.weight > b.weight then 
            return -1
        if a.weight < b.weight then 
            return 1
        return 0
    )
    
    w = (UEWeight 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0)
    skin = FindSkinModificator Mesh

    if skin != undefined then
    (    
        local VertexBoneCount = skinops.GetVertexWeightCount skin Vertex
        
        local ws = #()

        ws[1] = (temp_weight 0.0 0.0)
        ws[2] = (temp_weight 0.0 0.0)
        ws[3] = (temp_weight 0.0 0.0)
        ws[4] = (temp_weight 0.0 0.0)

        for Bone = 1 to VertexBoneCount do
        (        
            BoneName = skinOps.GetBoneName skin (skinOps.GetVertexWeightBoneID skin Vertex Bone) 0
            BoneId = FindBoneID BoneName Bones        
            Weight = skinops.getvertexweight skin Vertex Bone
            
            ws[Bone] = (temp_weight Weight BoneId)
        )

        if VertexBoneCount > 4 then
        	qsort ws CompareBones
		
		w = (UEWeight ws[1].id ws[2].id ws[3].id ws[4].id ws[1].weight ws[2].weight ws[3].weight ws[4].weight)		
	
   --     if VertexBoneCount == 3 then		
--			w = (UEWeight ws[1].id ws[2].id ws[3].id 0.0 ws[1].weight ws[2].weight ws[3].weight 0.0)

  --      if VertexBoneCount == 2 then
	--		w = (UEWeight ws[1].id ws[2].id 0.0 0.0 ws[1].weight ws[2].weight 0.0 0.0)
		
	--	if VertexBoneCount == 1 then
	--		w = (UEWeight ws[1].id 0.0 0.0 0.0 ws[1].weight 0.0 0.0 0.0)
			
	--	if VertexBoneCount == 0 then
		--	w = (UEWeight 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0)			
        
    )
    return w;
)

function GetBoneTransform Bone =
(
	at time animationrange.start (m0 = (inverse (Bone.objectTransform)))
	if ( Bone.Parent == undefined ) then
	(
		return m0 * Bone.objectTransform
	)
	
	return m0 * Bone.objectTransform -- * inverse(Bone.Parent.objecttransform)
)

function WriteBones File Bones =
(    
    -- UEngine id animation File    
    WriteLong File 121

    --skin = FindSkinModificator Mesh

    --if (Bones.count > 0) then
    (        
        Writelong File Bones.count
        
        astart = animationrange.start
        aend = animationrange.end
        
        Writelong File (aend-astart +1)        
        
        for f = astart to aend by 1 do 
        at time f
        (
            for Bone in Bones do
            (                
                Writestring File Bone.Name
                BoneIdParent = -1
                --BoneTransform = Bone.objectTransform

                --at time astart (m0 = (inverse (Bone.objectTransform)))
                    
                --if (Bone.Parent != undefined) then
                --(
                --    at time astart  (m1 = (inverse (Bone.Parent.objectTransform)))
                --   BoneParentTransform = (m1 * Bone.Parent.objectTransform)

                --    BoneIdParent = FindBoneId Bone.Parent.Name Bones
                
                    --Relative style
               --     BoneTransform = BoneTransform * (inverse (BoneParentTransform))
                    --at time astart  (m0 = (inverse (Bone.objectTransform * (inverse (m1 * Bone.Parent.objectTransform)))))
                --)                            
                
				--BoneTransform = m0 * bone.objectTransform
				
                WriteLong File BoneIdParent
                BoneTransform = ConvertMatrixToRightHanded (GetBoneTransform(Bone))
                
                p = BoneTransform.position
                s = BoneTransform.scale
                r = BoneTransform.rotation
                
                WriteFloat File (p.x)
                WriteFloat File (p.y)
                WriteFloat File (p.z)
                
                WriteFloat File (r.x)
                WriteFloat File (r.y)
                WriteFloat File (r.z)
                WriteFloat File (r.w)

                WriteFloat File s.x
                WriteFloat File s.y
                WriteFloat File s.z
            )            
        )    
        
    )
    --else
    --    Writelong File 0
)

function ConvertObjectToUEMesh Obj &ConvertedUEMesh SelectedIndex Bones =
(
    if superClassOf Obj != GeometryClass or not isValidNode Obj  do
        return false

    ConvertedUEMesh.Name =  Obj.Name
    
    local cMesh = snapshotAsMesh Obj
    
    if cMesh.numverts == 0 or cMesh.numfaces == 0 do
    (
        messageBox "Invalid geometry." title:"Error"
        return false
    )
        
    if cMesh.numtverts > 0 and cMesh.numtverts < cMesh.numverts do
    ( messageBox "Non zero number of UV coords is less then number of vertices.\n"+
        (cMesh.numtverts as string) + " < " + (cMesh.numverts as string)  title:"Warning")
    
    ConvertedUEMesh.UniqueVertices = cMesh.numverts;

    -- Material
    if Obj.Material != undefined and classof Obj.Material == StandardMaterial then
    (
        ConvertedUEMesh.Material.two_sided = 0

        if Obj.Material.twoSided do
            ConvertedUEMesh.Material.two_sided = 1

        ConvertedUEMesh.Material.dr = Obj.Material.diffuse.r
        ConvertedUEMesh.Material.dg = Obj.Material.diffuse.g
        ConvertedUEMesh.Material.db = Obj.Material.diffuse.b
        ConvertedUEMesh.Material.da = 255.0*(Obj.Material.opacity/100.0)
    
        ConvertedUEMesh.Material.spr = Obj.Material.specular.r
        ConvertedUEMesh.Material.spg = Obj.Material.specular.g
        ConvertedUEMesh.Material.spb = Obj.Material.specular.b
                
        ConvertedUEMesh.Material.ar = Obj.Material.ambient.r
        ConvertedUEMesh.Material.ag = Obj.Material.ambient.g
        ConvertedUEMesh.Material.ab = Obj.Material.ambient.b
    
        ConvertedUEMesh.Material.shin = 255.0*(Obj.Material.glossiness/100.0)
        
    )
    else
        ConvertedUEMesh.Material = (UEMaterial 200 200 200 255 200 200 200 100 100 100 100 1)
    
    -- Faces
    local dsign = Obj.transform.determinantsign
    local ib = #(1, 3, 2)
    
    if dsign < 0 do 
        ib = #(1, 2, 3)

    local cface = #(0, 0, 0)
    if cMesh.numtverts != 0 then
    (
        for i = 1 to cMesh.numfaces do
        (
            local f = getTVFace cMesh i
            cface = #(f.x-1, f.y-1, f.z-1)
            ConvertedUEMesh.FaceBuffer[i] = (UEFace cface[ib[1]] cface[ib[2]] cface[ib[3]])
        )
    )
    else
    (
        for i = 1 to cMesh.numfaces do
        (
            local f = getFace cMesh i
            cface = #(f.x-1, f.y-1, f.z-1)
            ConvertedUEMesh.FaceBuffer[i] = (UEFace cface[ib[1]] cface[ib[2]] cface[ib[3]])
        )
    )
    
    skin = FindSkinModificator Obj
    if skin != undefined then 
        modPanel.setCurrentObject skin

    -- Vertices
    if cMesh.numtverts > cMesh.numverts then
    (
        -- Conversion vertices
        local temp_buf = #()
        for i = 1 to cMesh.numfaces do
        (
            local f_uv = getTVFace cMesh i
            local f_vert = getFace cMesh i
            temp_buf[f_uv.x] = f_vert.x
            temp_buf[f_uv.y] = f_vert.y
            temp_buf[f_uv.z] = f_vert.z
        )
            
        for i = 1 to cMesh.numtverts do
        (
            local v = getVert cMesh temp_buf[i]
            local n = getNormal cMesh temp_buf[i]
            local uv = getTVert cMesh i
            local w = getWeightData temp_buf[i] Obj Bones
            
            ConvertedUEMesh.VerticesBuffer[i] = (UEVertex v.x v.y v.z n.x n.y n.z uv.x (1.0-uv.y) w)
        )
    )
    else
    (    
        for i = 1 to cMesh.numverts do
        (
            local v = getVert cMesh i
            local n = getNormal cMesh i
            local w = getWeightData i Obj Bones

            if cMesh.numtverts==0 then
                ConvertedUEMesh.VerticesBuffer[i] = (UEVertex v.x v.y v.z n.x n.y n.z 0.0 0.0 w)
            else
            (
                local uv = getTVert cMesh i
                ConvertedUEMesh.VerticesBuffer[i] = (UEVertex v.x v.y v.z n.x n.y n.z uv.x (1.0-uv.y) w)
            )
        )
    )
    
    -- transform verts to right handed
    ConvertVerticesToRightHanded ConvertedUEMesh.VerticesBuffer
    
    -- compute Mesh data size in bytes
    ConvertedUEMesh.DataSize = ConvertedUEMesh.VerticesBuffer.count * (64) + ConvertedUEMesh.FaceBuffer.count * 6 +  11 + ConvertedUEMesh.Name.count + 1 + 4
    
    return true
)


function WriteMesh &c_Mesh &File_out = 
(
    WriteLong File_out c_Mesh.DataSize

    WriteLong File_out c_Mesh.VerticesBuffer.count
    WriteLong File_out c_Mesh.FaceBuffer.count
    
    WriteString File_out c_Mesh.Name
    
    WriteByte File_out c_Mesh.Material.dr
    WriteByte File_out c_Mesh.Material.dg
    WriteByte File_out c_Mesh.Material.db
    WriteByte File_out c_Mesh.Material.da
    
    WriteByte File_out c_Mesh.Material.two_sided
    
    WriteByte File_out c_Mesh.Material.ar
    WriteByte File_out c_Mesh.Material.ag
    WriteByte File_out c_Mesh.Material.ab
    
    WriteByte File_out c_Mesh.Material.spr
    WriteByte File_out c_Mesh.Material.spg
    WriteByte File_out c_Mesh.Material.spb
    
    WriteByte File_out c_Mesh.Material.shin
        
    for i = 1 to c_Mesh.FaceBuffer.count do
    (
        WriteShort File_out c_Mesh.FaceBuffer[i].v0
        WriteShort File_out c_Mesh.FaceBuffer[i].v1
        WriteShort File_out c_Mesh.FaceBuffer[i].v2
    )
    
    for i = 1 to c_Mesh.VerticesBuffer.count do
    (
        WriteFloat File_out c_Mesh.VerticesBuffer[i].x
        WriteFloat File_out c_Mesh.VerticesBuffer[i].y
        WriteFloat File_out c_Mesh.VerticesBuffer[i].z
        WriteFloat File_out c_Mesh.VerticesBuffer[i].nx
        WriteFloat File_out c_Mesh.VerticesBuffer[i].ny
        WriteFloat File_out c_Mesh.VerticesBuffer[i].nz
        WriteFloat File_out c_Mesh.VerticesBuffer[i].u
        WriteFloat File_out c_Mesh.VerticesBuffer[i].v
        
		i0 = ConvertOkToFloat(c_Mesh.VerticesBuffer[i].w.i0)
		i1 = ConvertOkToFloat(c_Mesh.VerticesBuffer[i].w.i1)
		i2 = ConvertOkToFloat(c_Mesh.VerticesBuffer[i].w.i2)
		i3 = ConvertOkToFloat(c_Mesh.VerticesBuffer[i].w.i3)
		
        WriteFloat File_out i0
        WriteFloat File_out i1
        WriteFloat File_out i2
        WriteFloat File_out i3
    
        WriteFloat File_out c_Mesh.VerticesBuffer[i].w.w0
        WriteFloat File_out c_Mesh.VerticesBuffer[i].w.w1
        WriteFloat File_out c_Mesh.VerticesBuffer[i].w.w2
        WriteFloat File_out c_Mesh.VerticesBuffer[i].w.w3
        
    )
    
)
function exportBones SelectedIndex = 
(   
    Bones = #()
    FindAllBones &Bones

    File_Name = GetSaveFileName \
    caption:"Select File To Export" \
    FileName:"Untitled" \
    types:"UEngine Model Animation (*.uma)|*.uma|"
    
    if File_Name == undefined do
        return true

    File_out = fopen File_Name "wb"
    
    if File_out == undefined do
        return false

    WriteBones File_out Bones
    
    fclose File_out
    
    messageBox ("Bones animation exported!\n") title: "Result"
            
    return true
)

function exportObject SelectedIndex =
(
    Bones = #()
    FindAllBones &Bones

    local s = selection
    select selection[SelectedIndex]
    max modify mode
    
    local c_Mesh = (UEMesh  "object" #() #() (UEMaterial 0 0 0 0 0) 0 0)
    if not (ConvertObjectToUEMesh selection[SelectedIndex] &c_Mesh SelectedIndex Bones) do
        return false
    
    File_Name = GetSaveFileName \
    caption:"Select File To Export" \
    FileName:"Untitled" \
    types:"UEngine Polygonal Model (*.upm)|*.upm|"
    
    if File_Name == undefined do
        return true

    File_out = fopen File_Name "wb"

    if File_out == undefined do
        return false
    
    -- Write header
    WriteLong File_out 111 -- id model File    
    WriteLong File_out 1 -- Mesh count
    
    WriteMesh c_Mesh File_out
    
    -- close File
    fclose File_out
    
    -- show result
    messageBox ("Model exported!\n") title: "Result"
                
    selection = s
    
    if UtilDialog.oneframe.state then
    (
        exportBones SelectedIndex
    )
    
    return true
)

function exportAll n =
(
    Bones = #()
    FindAllBones &Bones

    File_Name = GetSaveFileName \
    caption:"Select File To Export" \
    FileName:"Untitled" \
    types:"UEngine Polygonal Model (*.upm)|*.upm|"
    
    if File_Name == undefined do
        return true
    
    File_out = fopen File_Name "wb"
    
    if File_out == undefined do
        return false
    
    -- Write header
    WriteLong File_out 111 -- id model File
    
    WriteLong File_out selection.count
    
    local Mesh_array = #()
    for i = 1 to selection.count do
        Mesh_array[i] = selection[i].Name
    
    for m = 1 to Mesh_array.count do
    (
        --local s = selection
        select (getnodebyName Mesh_array[m])
        max modify mode
        
        --if !(matchPattern selection[m].Name  pattern:"Bone?") then
        --continue
        -- convert object

        local c_Mesh = (UEMesh "object" #() #() (UEMaterial 0 0 0 0 0 0 0 0 0 0 0 0) 0 0)
        
        if not (ConvertObjectToUEMesh selection[1] &c_Mesh m Bones) do
            return false
        
        WriteMesh c_Mesh File_out        
        --selection = s
        
    )
    
    -- close File
    fclose File_out
    
    messageBox ("Models exported!\n") title: "Result"
    
    if UtilDialog.oneframe.state then
    (
        select (getnodebyName Mesh_array[1])
        max modify mode

        exportBones 1
    )
    
    return true
)

utility UtilDialog "UEngine Mesh Export Utility" width:300 height:300
(
    label uiLabelInfo "Exports scene objects as .UPM\n(UEngine Polygonal Model) File." align:#left width:290 height:40
    group "Objects"
    (
        label uiLabelList "Select object to export" align:#left
        listbox uiSelList /*items:#("1","2","3")*/ height:10
    )
    group "Parameters" 
    (
        checkbox oneframe "Save Animation" checked:true align:#center across:1
    --    spinner stepframe "Frame Step" range:[1,200,1] fieldwidth:40 type:#integer align:#center
    --    radiobuttons useanim labels:#("Active Time Segment", "Custom Time Segment")

--        spinner animstart range:[0,1000,0] type:#integer fieldwidth:40 across:1
--        spinner animend "to" range:[0,1000,40] type:#integer fieldwidth:40

    )
    group "Export"
    (

        button uiBtnExport "Export Selected"  width:220 height:25
        button uiBtnExportAll "Export All"  width:220 height:25
        button uiBtnExportBones "Export Bones Animation"  width:220 height:25

    )
    
    button uiBtnClose "Close"  width:100 height:25
    
    on UtilDialog open do
    (
        local i
        local Mesh_array=#()-- = selection as array
        for i = 1 to selection.count do
            Mesh_array[i] = selection[i].Name
        
        if selection.count==0 do
        (
            uiBtnExport.enabled = false
            uiBtnExportAll.enabled = false
            uiBtnExportBones.enabled = false
        )
        
        uiSelList.items = Mesh_array
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
    on uiBtnExportBones pressed do
    (
        if not exportBones uiSelList.selection then
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

CreateDialog  UtilDialog width:320 height:425
