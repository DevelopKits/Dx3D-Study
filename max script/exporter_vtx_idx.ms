
global Outfile -- ����� file
global g_FilePath

global g_varSetMeshes = #()


fn ConvertToD3D_POS max_pos =
(
	local d3d_pos = [0,0,0]
	d3d_pos.x = max_pos.x
	d3d_pos.y = max_pos.z
	d3d_pos.z = max_pos.y
	
	d3d_pos
)


fn ConvertToD3D_INDEX max_index =
(
	local d3d_index = [0,0,0]
	d3d_index.x = max_index.x - 1
	d3d_index.y = max_index.z - 1
	d3d_index.z = max_index.y - 1

	d3d_index
)


fn CollectObject = 
(
	g_varSetMeshes = #()

	for i=1 to objects.count do
	(
		append g_varSetMeshes objects[i]
	)
)


fn CollectVertex obj = 
(
	varSetVertex= #()
	for i=1 to obj.numverts do
	(
		v = getvert obj i
		append varSetVertex v
	)
	varSetVertex
)

fn CollectIndex obj = 
(
	varSetIndex = #()
	for i=1 to obj.numfaces do
	(
		v = copy (getface obj i)
		append varSetIndex v
	)
	varSetIndex
)


fn WriteVertex obj = 
(
	varSetVertice = CollectVertex obj

	format "VERTEX = %\n" varSetVertice.count to:Outfile
	for i=1 to varSetVertice.count do
	(
		vtx = (ConvertToD3D_POS varSetVertice[ i]) 
		format "% % %\n" vtx.x vtx.y vtx.z to:Outfile
	)
)


fn WriteIndex obj = 
(
	varSetIndex = CollectIndex obj

	format "INDEX = %\n" varSetIndex.count to:Outfile
	for i=1 to varSetIndex.count do
	(
		index = (ConvertToD3D_INDEX varSetIndex[ i])
		format "% % %\n" (index.x as integer) (index.y as integer) (index.z as integer) to:Outfile
	)
)


fn ExporterStart = 
(
	CollectObject()

	Outfile = createfile g_FilePath

	for i=1 to g_varSetMeshes.count do
	(
		print g_varSetMeshes[i] 
		print g_varSetMeshes[i].name 
		
		-- vertex, index���� EditMesh����϶��� ���ü� �־ ConvertToMesh �Լ���
		-- ����ߴ�. ������ EditMesh����϶� VertexWeight���� ���ü� ���ٴ°��̴�.
		-- �׷��� Vertex, index���� ���� �� undo���ɾ�� �������·� �ٲ��� ���´�.
		undo on
		(
			ConvertToMesh g_varSetMeshes[ i]
			WriteVertex g_varSetMeshes[ i]
			WriteIndex g_varSetMeshes[ i]
		)
		max undo
	)

	close Outfile

)



rollout ui "�ȳ�" width:387 height:266
(
	button btnRun "DX Exporter ����" pos:[101,108] width:177 height:30

	-- Bmm, Ani ���ϸ� �Է�â
	label lbl1 "���ϸ�:" pos:[30,13] width:70 height:17
	edittext editFilePath "" pos:[100,10] width:250 height:17 enabled:true

	-- UI â�� ������ 
	on ui open do
	(
		editFilePath.text = "data.txt"
	)

	-- DirectX Export
	on btnRun pressed do
	(
		g_FilePath = editFilePath.text
		ExporterStart()
	)

)

MainUI = newRolloutFloater "Simple Exporter" 400 300
addrollout ui MainUI
