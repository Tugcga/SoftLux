#include "lux_session.h"
#include "../../utilities/logs.h"

#include <filesystem>

void update_gpu_kernels(luxcore::RenderConfig* render_config, const int engine)
{
	if (engine == 2 && !render_config->HasCachedKernels())
	{
		//the system does not find the kernel file
		
		//try to copy files from temp Luxcore directory
		std::filesystem::path xsi_temp_directory = std::filesystem::temp_directory_path();
		std::filesystem::path luxcore_directory = xsi_temp_directory.parent_path().parent_path() / "luxcorerender.org";
		xsi_temp_directory = xsi_temp_directory / "luxcorerender.org";
		bool is_luxcore_exist = std::filesystem::exists(luxcore_directory);
		if (is_luxcore_exist)
		{
			//temp folder for Luxcore exist
			//copy files
			std::filesystem::copy(luxcore_directory, xsi_temp_directory, std::filesystem::copy_options::recursive);
		}

		//try to find cached kernel again
		if (!render_config->HasCachedKernels())
		{
			//kernel does not find again
			//so, we should generate it and then copy to the luxcore temp folder (to use in the next Softimage start)

			//but first we should remember what files already exists in the folder, and then copy only new ones
			std::set<std::string> xsi_old_files;
			for (const auto& p : std::filesystem::recursive_directory_iterator(xsi_temp_directory))
			{
				if (!std::filesystem::is_directory(p))
				{
					XSI::CString xsi_str(p.path().c_str());
					xsi_old_files.insert(std::string(xsi_str.GetAsciiString()));
				}
			}

			log_message("The system should compile gpu kernels, wait several minutes", XSI::siWarningMsg);

			//compile kernels
			luxrays::Properties config_props_copy = luxrays::Properties(render_config->GetProperties());
			config_props_copy.Set(luxrays::Property("kernelcachefill.renderengine.types")("PATHOCL"));  // by default Luxcore compile for "PATHOCL", "TILEPATHOCL", "RTPATHOCL"
			luxcore::KernelCacheFill(config_props_copy);

			const auto copy_options = std::filesystem::copy_options::overwrite_existing;
			//find new files int the xsi temp directory
			for (const auto& p : std::filesystem::recursive_directory_iterator(xsi_temp_directory))
			{
				if (!std::filesystem::is_directory(p))
				{
					XSI::CString xsi_str(p.path().c_str());
					std::string std_str = std::string(xsi_str.GetAsciiString());

					if (!xsi_old_files.contains(std_str))
					{
						//this is a new file
						//get file relative path
						std::filesystem::path r = std::filesystem::relative(std_str, xsi_temp_directory);
						//append to the luxcore path
						std::filesystem::path to_path = luxcore_directory / r;
						//if file does not exists in the luxcore directory
						if (!std::filesystem::exists(to_path))
						{
							std::filesystem::path to_path_dir = to_path.parent_path();
							bool is_folder_exist = false;
							//check is parent folder for the target file is created
							if (std::filesystem::exists(to_path_dir))
							{
								is_folder_exist = true;
							}
							else
							{
								//create it
								if (std::filesystem::create_directories(to_path_dir))
								{
									is_folder_exist = true;
								}
							}

							if (is_folder_exist)
							{
								//copy the file
								std::filesystem::copy_file(std_str, to_path, copy_options);
							}
						}

						//finish copy one new file
					}

					//finish iterate by files in temp xsi directory
				}
			}

			xsi_old_files.clear();
			//finish copy files from temp XSI directory to the temp Luxcore directory
			//finish second check of kernels
		}
		//finish first chen of kernels
	}
}