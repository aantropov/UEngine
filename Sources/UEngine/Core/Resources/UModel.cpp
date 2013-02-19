#include "UModel.h"
#include "..\Utils\utils.h"
#include "UTexture.h"
#include "stdio.h"

void UModel:: Render(URENDER_TYPE type){

	URenderer::GetInstance()->PushModelMatrix();
	m.Set();
	for(int i = mesh_num-1; i >= 0; i--)
	{	
		if(countPlayableAnimations > 0)
		{			
			meshes[i]->material.SetSkinningMatrixes(boneMatrixes, currentFrame.bones.size());
		}
		meshes[i]->Render(type);
	}
	URenderer::GetInstance()->PopModelMatrix();
}

void UModel:: Render(UMaterial *mat){

	URenderer::GetInstance()->PushModelMatrix();
	m.Set();
	
	if(countPlayableAnimations > 0)
		mat->SetSkinningMatrixes(boneMatrixes, currentFrame.bones.size());

	for(int i = mesh_num-1; i >= 0; i--)
		meshes[i]->Render(mat);
	URenderer::GetInstance()->PopModelMatrix();
}
void UModel:: Update(double delta){

	UComponent::Update(delta);
	for(unsigned int i = 0; i < mesh_num; i++)
			meshes[i]->Update(delta);
}

void UModel:: Free(){
	for(unsigned int i = 0; i < mesh_num; i++)
		delete meshes[i];
	meshes.clear();
	mesh_num = 0;
}

bool UModel:: Load(std:: string path){
	
	UXMLFile xml;
	
	char buffer[UE_MAXCHAR];
	char tex_buffer[UE_MAXCHAR];

	try{
		xml.Load(path);
		this->name = xml.GetElement("/xml/model/name/");
		
		if(xml.isExistElement("/xml/model/upm/"))
		{
			int data_size = 0;
			int i_buffer = 0;	

			FILE* file = NULL;
			fopen_s(&file, xml.GetElement("/xml/model/upm/").c_str(), "rb");

			if(file == NULL)
			{
				ULogger::GetInstance()->Message("Cannot find file: " + xml.GetElement("/xml/model/upm/"));
				throw;
			} 

			fread(&i_buffer, 4, 1, file);
			if(i_buffer != 111)
			{
				ULogger::GetInstance()->Message("Wrong upm format: " + xml.GetElement("/xml/model/upm/"));
				throw;
			}

			fread(&i_buffer, 4, 1, file);

			this->mesh_num = i_buffer;
			
			for(unsigned int i = 0; i < mesh_num; i++){

				UMesh* mesh = new UMesh();

				fread(&data_size, 4, 1, file);
			
				fread(&i_buffer, 4, 1, file);
				mesh->vb.Create(i_buffer);

				fread(&i_buffer, 4, 1, file);
				mesh->ib.Create(i_buffer);

				char c_buffer[MAXCHAR];
				memset(buffer, 0, MAXCHAR);

				int count = 0;
				do
				{
					fread(&c_buffer[count],1,1,file);
					count++;
				}
				while(c_buffer[count-1] != '\0');

				mesh->name = string(c_buffer);

				bool finded = false;
				std::string current_mesh;
				int xml_mesh_num = atoi(xml.GetElement("/xml/model/mesh_num/").c_str());
				for(int j = 0; j < xml_mesh_num; j++)
				{
					memset(buffer,'\0',255);
					sprintf_s(buffer,"%d",j);				
					current_mesh = "/xml/model/mesh_" + string(buffer) + "/";
					if(xml.GetElement(current_mesh + "name/") == mesh->name)
					{
						finded = true;
						break;
					}
				}

				unsigned char ar, ab, ag;
				unsigned char dr, db, dg, da;
				unsigned char sr, sb, sg;
				unsigned char shin;
				unsigned char two_sides;

				fread(&dr, 1,1, file);
				fread(&dg, 1,1, file);
				fread(&db, 1,1, file);
				fread(&da, 1,1, file);

				fread(&two_sides, 1,1, file);

				fread(&ar, 1,1, file);
				fread(&ag, 1,1, file);
				fread(&ab, 1,1, file);
				
				fread(&sr, 1,1, file);
				fread(&sg, 1,1, file);
				fread(&sb, 1,1, file);
				
				fread(&shin, 1,1, file);
				

				mesh->material = UMaterial( vec4(color(ar, ag, ab, 255)),
											vec4(color(dr, dg, db, da)),
											vec4(color(sr, sg, sb, 255)),
											vec4_zero,
											//vec4(color(ar, ag, ab, 255)),
											((float)shin)/255.0f);

				unsigned int *m_indices = reinterpret_cast<unsigned int*>(mesh->ib.GetPointer());
				for(int j = 0; j < mesh->ib.GetNum(); j++)
				{	
					short ind;
					fread(&ind, 2, 1, file);
					m_indices[j] = ind;
				}
				//fread(m_indices, 2, mesh->ib.GetNum(), file);
			
				UVertex *m_vertices = reinterpret_cast<UVertex*>(mesh->vb.GetPointer());
				for(int j = 0; j < mesh->vb.GetNum(); j++)
				{
					float x, y, z;
					float nx, ny, nz;
					float u, v;
					vec4 ind;
					vec4 weight;

					fread(&x, 4,1, file);
					fread(&y, 4,1, file);
					fread(&z, 4,1, file);

					fread(&nx, 4,1, file);
					fread(&ny, 4,1, file);
					fread(&nz, 4,1, file);

					fread(&u, 4,1, file);
					fread(&v, 4,1, file);

					fread(ind.v, 4,4, file);
					fread(weight.v, 4,4, file);

					//ind = vec4(1.0f, 0.0f, 0.0f, 0.0f);
					//weight = vec4(1.0f, 0.0f, 0.0f, 0.0f);
					//ind += vec4(1.0f, 1.0f, 1.0f, 1.0f)*0.1f;
				//	weight.y = weight.z = weight.w;vec4(0.0f, 0.0f, 1.0f, 0.0f);
				//	ULogger::GetInstance()->Message(vertices[count] + " " + vertices[count+1] + " " + vertices[count+7], ULOG_MSG_INFO, ULOG_OUT_MSG);
					m_vertices[j] = UVertex(vec3(x, y, z), vec3(nx, ny ,nz), vec2(u, v));
					
				//	if(ind.x < 0.0f || ind.y < 0.0f || ind.z < 0.0f || ind.w < 0.0f)
					//	ind = vec4_zero;
					
					m_vertices[j].SetBoneIndex(ind);
					m_vertices[j].SetBoneWeight(normalize(weight));
				}

				if(finded)
				{
					if(xml.isExistElement(current_mesh+ "material/"))
					{
						mesh->material = *dynamic_cast<UMaterial*>(rf->Load(xml.GetElement(current_mesh+ "material/"), URESOURCE_MATERIAL));
					}
					else
					{
						mesh->material.SetShaderProgram(rf->Load(xml.GetElement(current_mesh + "shaders/vertex_path/"), xml.GetElement(current_mesh + "shaders/pixel_path/")), URENDER_FORWARD);

						int tex_num = atoi(xml.GetElement(current_mesh + "tex_num/").c_str());
						for(int j = 0; j < tex_num; j++)
						{
							memset(tex_buffer,'\0',255);	
							sprintf_s(tex_buffer,"%d",j);
							std::string current_tex = current_mesh+ "textures/tex_" + string(tex_buffer) + "/";
				
							auto tex = pair<UTexture*, unsigned int>( 
								dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), URESOURCE_TEXTURE)), 
								atoi(xml.GetElement(current_tex + "channel/").c_str()));
				
							tex.first->name = xml.GetElement(current_tex + "name/");
				
							mesh->material.AddTexture(tex);
						}
					}
				}
				else
				{
					if(xml.isExistElement("/xml/model/common_material/"))
					{
						mesh->material = *dynamic_cast<UMaterial*>(rf->Load(xml.GetElement("/xml/model/common_material/"), URESOURCE_MATERIAL));
					}
					else
					{
						mesh->material.SetShaderProgram(rf->Load(xml.GetElement("/xml/model/common_shaders/vertex_path/"), xml.GetElement("/xml/model/common_shaders/pixel_path/")), URENDER_FORWARD);

						int tex_num = atoi(xml.GetElement("/xml/model/tex_num/").c_str());
						for(int j = 0; j < tex_num; j++)
						{

							memset(tex_buffer,'\0',255);	
							sprintf_s(tex_buffer,"%d",j);
							std::string current_tex = "/xml/model/common_textures/tex_" + string(tex_buffer) + "/";
				
							auto tex = pair<UTexture*, unsigned int>( 
								dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), URESOURCE_TEXTURE)), 
								atoi(xml.GetElement(current_tex + "channel/").c_str()));
				
							tex.first->name = xml.GetElement(current_tex + "name/");
				
							mesh->material.AddTexture(tex);
						}
					}
				}

				if(xml.isExistElement("/xml/model/common_shaders/normal_vertex_path/") && xml.isExistElement("/xml/model/common_shaders/normal_pixel_path/"))
					mesh->material.SetShaderProgram(rf->Load(xml.GetElement("/xml/model/common_shaders/normal_vertex_path/"), xml.GetElement("/xml/model/common_shaders/normal_pixel_path/")), URENDER_NORMAL);
				
				if(xml.isExistElement("/xml/model/common_shaders/depth_vertex_path/") && xml.isExistElement("/xml/model/common_shaders/depth_pixel_path/"))
					mesh->material.SetShaderProgram(rf->Load(xml.GetElement("/xml/model/common_shaders/depth_vertex_path/"), xml.GetElement("/xml/model/common_shaders/depth_pixel_path/")), URENDER_DEPTH);
				
				if(xml.isExistElement("/xml/model/common_shaders/deffered_vertex_path/") && xml.isExistElement("/xml/model/common_shaders/deffered_pixel_path/"))
					mesh->material.SetShaderProgram(rf->Load(xml.GetElement("/xml/model/common_shaders/deffered_vertex_path/"), xml.GetElement("/xml/model/common_shaders/deffered_pixel_path/")), URENDER_DEFFERED);

				int anim_num = atoi(xml.GetElement("/xml/model/anim_num/").c_str());
				if(anim_num > 0)
				{
					for(int j = 0; j < anim_num; j++)
					{
						memset(tex_buffer,'\0',255);	
						sprintf_s(tex_buffer,"%d",j);
						std::string current_anim = "/xml/model/animations/animation_" + string(tex_buffer) + "/";
						animations[xml.GetElement(current_anim + "name/")] = dynamic_cast<UAnimation*>(rf->Load(xml.GetElement(current_anim + "path/"), URESOURCE_SKIN_ANIMATION));						
					}
				}

				mesh->Initialize();
				meshes.push_back(mesh);
			}
			fclose(file);
		}
		else
		{
			//Deprecated!!! 

			this->mesh_num = atoi(xml.GetElement("/xml/model/mesh_num/").c_str());

			for(unsigned int i = 0; i < mesh_num; i++){

				UMesh* mesh = new UMesh();
			
				memset(buffer,'\0',255);	
				sprintf_s(buffer,"%d",i);
				std::string current_mesh = "/xml/model/mesh_" + string(buffer) + "/";
						
				mesh->material.SetShaderProgram(rf->Load(xml.GetElement(current_mesh + "shaders/vertex_path/"), xml.GetElement(current_mesh + "shaders/pixel_path/")), URENDER_FORWARD);
					
				int tex_num = atoi(xml.GetElement(current_mesh + "tex_num/").c_str());
				for(int j = 0; j < tex_num; j++)
				{

					memset(tex_buffer,'\0',255);	
					sprintf_s(tex_buffer,"%d",j);
					std::string current_tex = current_mesh+ "textures/tex_" + string(tex_buffer) + "/";
				
					auto tex = pair<UTexture*, unsigned int>( 
						dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), URESOURCE_TEXTURE)), 
						atoi(xml.GetElement(current_tex + "channel/").c_str()));
				
					tex.first->name = xml.GetElement(current_tex + "name/");
				
					mesh->material.AddTexture(tex);
				}

				mesh->name = xml.GetElement(current_mesh + "name/");
				mesh->ib.Create(atoi(xml.GetElement(current_mesh + "faces_num/").c_str()));			
				std::vector<std::string> indices = Split(xml.GetElement(current_mesh + "indices/"));
				unsigned int *m_indices = reinterpret_cast<unsigned int*>(mesh->ib.GetPointer());
				for(int j = 0; j < mesh->ib.GetNum(); j++)
					m_indices[j] = atoi(indices[j].c_str());

				int count = 0;
				mesh->vb.Create(atoi(xml.GetElement(current_mesh + "verts_num/").c_str()));
				std::vector<std::string> vertices = Split(xml.GetElement(current_mesh + "vertices/"));			
				UVertex *m_vertices = reinterpret_cast<UVertex*>(mesh->vb.GetPointer());
				for(int j = 0; j < mesh->vb.GetNum(); j++)
				{
				//	ULogger::GetInstance()->Message(vertices[count] + " " + vertices[count+1] + " " + vertices[count+7], ULOG_MSG_INFO, ULOG_OUT_MSG);
					m_vertices[j] = UVertex( vec3((float)atof(vertices[count].c_str()), (float)atof(vertices[count+1].c_str()), (float)atof(vertices[count+2].c_str())),
										   vec3((float)atof(vertices[count+3].c_str()), (float)atof(vertices[count+4].c_str()), (float)atof(vertices[count+5].c_str())),
										   vec2((float)atof(vertices[count+6].c_str()), (float)atof(vertices[count+7].c_str())) );
					count += 8;
				}

				std::vector<std::string> ambient = Split(xml.GetElement(current_mesh + "material/ambient/"));
				std::vector<std::string> diffuse = Split(xml.GetElement(current_mesh + "material/diffuse/"));
				std::vector<std::string> specular = Split(xml.GetElement(current_mesh + "material/specular/"));
				std::vector<std::string> emission = Split(xml.GetElement(current_mesh + "material/emission/"));

				mesh->material = UMaterial( vec4((float)atof(ambient[0].c_str()), (float)atof(ambient[1].c_str()), (float)atof(ambient[2].c_str()), (float)atof(ambient[3].c_str())),
											vec4((float)atof(diffuse[0].c_str()), (float)atof(diffuse[1].c_str()), (float)atof(diffuse[2].c_str()), (float)atof(diffuse[3].c_str())),
											vec4((float)atof(specular[0].c_str()), (float)atof(specular[1].c_str()), (float)atof(specular[2].c_str()), (float)atof(specular[3].c_str())),
											vec4((float)atof(emission[0].c_str()), (float)atof(emission[1].c_str()), (float)atof(emission[2].c_str()), (float)atof(emission[3].c_str())),
											(float)atof(xml.GetElement(current_mesh + "material/shininess/").c_str()));
				mesh->Initialize();
				meshes.push_back(mesh);
			}
		}				

	}catch(exception e){
		ULogger::GetInstance()->Message("Error to load model (xml): " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
		return false;
	}
	return true;
}
