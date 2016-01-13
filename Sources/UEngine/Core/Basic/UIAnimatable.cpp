#include "UIAnimatable.h"
#include "..\Resources\UXMLFile.h"

bool UAnimation::Load(UXMLFile& xml, std::string path)
{
    char buffer[UE_MAXCHAR];
    try
    {
        this->name = xml.GetElement(path + "animation/name/");

        if (xml.isExistElement(path + "animation/uma/"))
        {
            unsigned int total_bone_count = 0;
            int frame_count = 0;
            int i_buffer = 0;

            FILE* file = NULL;
            fopen_s(&file, xml.GetElement(path + "animation/uma/").c_str(), "rb");

            if (file == NULL)
            {
                ULogger::GetInstance()->Message("Cannot find file: " + xml.GetElement(path + "animation/uma/"));
                throw;
            }

            fread(&i_buffer, 4, 1, file);
            if (i_buffer != 121)
            {
                ULogger::GetInstance()->Message("Wrong uma format: " + xml.GetElement(path + "animation/uma/"));
                throw;
            }

            fread(&total_bone_count, 4, 1, file);
            fread(&frame_count, 4, 1, file);

            for (unsigned int i = 0; i < (unsigned int)frame_count; i++)
            {
                UKeyFrame frame;

                for (unsigned int j = 0; j < total_bone_count; j++)
                {
                    UBone bone;

                    char c_buffer[MAXCHAR];
                    memset(buffer, 0, MAXCHAR);

                    int count = 0;
                    do
                    {
                        fread(&c_buffer[count], 1, 1, file);
                        count++;
                    } while (c_buffer[count - 1] != '\0');

                    bone.name = string(c_buffer);

                    fread(&bone.parent, 4, 1, file);
                    fread(&bone.tf.position, 4, 3, file);
                    fread(&bone.tf.rotation, 4, 4, file);
                    fread(&bone.tf.scale, 4, 3, file);

                    frame.bones.push_back(bone);
                }
                frames.push_back(frame);
            }

            this->frameRate = atoi(xml.GetElement(path + "animation/frame_rate/").c_str());
            fclose(file);
        }

    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load animation (xml): " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
        return false;
    }

    return true;
}