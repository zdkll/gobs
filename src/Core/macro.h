#ifndef MACRO_H
#define MACRO_H

//Basic Defines-----------------------------------
#define PI                     3.141593
//项目文件后缀名
#define Project_File_Suffix    ".gpro"
#define Project_Download_Suffix ".download"
//项目文件内容标识
#define Project_Indentify      "gobs_project"

//工区文件后缀名
#define Area_File_Suffix       ".area"

//文件名非法字符
#define ILLEGAL_CHARS    "[\\\\/:*?\"<>|\\[\\]#@!~`,'\\{\\}\\+  \\(\\)%\\$\\^&=]"
#define Dir_Separator    "/"

#define Station_Rcv_Id     'R'
#define Station_Shot_Id    'S'
#define Station_X_Id       'X'

#define Node_Id            "G"

//Json Defines====================================
#define Json_Key_Project_Indentity       "Id"
#define Json_Value_Project_Indentity     Project_Indentify
#define Json_Key_Project_Areas           "Areas"
#define Json_Key_Project_Data_Path       "DataPath"
#define Json_Key_Project_Work_Path       "WorkPath"
#define Json_Key_Project_Download        "Download File"

#define Json_Key_Area_Indentity          "Id"
#define Json_Value_Area_Indentity        "gobs_area"
#define Json_Key_Area_SPS_Files          "SPS_Files"

//----------------------------------------------
#define SegyTextHeaderFile_Suffix        "_textheader"
#define ShotLineFiles_Suffix             "_shotLines"

/*===============ICON==============================*/
#define  ICON_NEW_FILE           "/icon/new_file.png"
#define  ICON_OPEN_FILE          "/icon/open_file.png"
#define  ICON_CLOSE              "/icon/close.png"
#define  ICON_UPDATE             "/icon/update.png"
#define  ICON_DELETE             "/icon/delete.png"
#define  ICON_COLOR_PREFERENCE   "/icon/color_preference.png"
#define  ICON_ZOOM_IN            "/icon/zoom_in.png"
#define  ICON_ZOOM_OUT           "/icon/zoom_out.png"
#define  ICON_ZOOM_IN_FULL       "/icon/zoom_in_full.png"
#define  ICON_ZOOM_OUT_FULL      "/icon/zoom_out_full.png"
#define  ICON_ITEM_LIST          "/icon/list_item.png"

//----------------------------ICON-----------------------------------
#define ICON_SET_FILE            "/icon/setting.png"
#define ICON_SEARCH_FILE         "/icon/search.png"
#define ICON_UPDATE_FILE         "/icon/update.png"
#define ICON_START_FILE          "/icon/start.png"
#define ICON_DOWNLOAD_FILE       "/icon/data_download.png"

#define ICON_START               "/icon/start.png"
#define ICON_STOP                "/icon/stop.png"
#define ICON_SEARCH_SET          "/icon/search.png"
#define ICON_DATA_MANAGER        "/icon/data_manager.png"

//--------------------------ICON DMC------------------------------
#define ICON_PREVIOUS            "/icon/previous.png"
#define ICON_NEXT                "/icon/next.png"
#define ICON_MOVE_TO             "/icon/move.png"

#define ICON_SETTINGS            "/icon/settings"
#define ICON_INFORMATION         "/icon/information.png"

//ExportSegy Job Parameter keys---------------
#define Json_Key_JobType         "JobType"
#define Json_Key_JobName         "JobName"
#define Json_Key_ProjectPath     "ProjectPath"
#define Json_Key_ProjectName     "ProjectName"
#define Json_key_AreaName        "AreaName"

#define Json_key_NS              "NS"
#define Json_key_DS              "DS"
#define Json_key_DataPath        "DataPath"
#define Json_key_OutputPath      "OutputPath"
#define Json_key_Shot_SourceType "Shot_SourceType"
#define Json_key_LineScope       "LineScope"
#define Json_key_StationScope    "StationScope"

#define Json_key_ShotLineFiles   "ShotLineFiles"
#define Json_key_SegyTextHeader  "SegyTextHeader"

#endif // MACRO_H

