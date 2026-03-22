    #pragma once
     
    /*
      Adjustable FOV
    ======================================================
    Allows the user to enhance the game's field of view
    through the options menu, through a command and in
    the config.xml
    ======================================================
    Addon Name: AdjustableFOV
    Author: Franc[e]sco
    */
     
    #include "MXml.h"
    #include "ZIDLResource.h"
    #include "ZChatCmd.h"
     
    // ZCONFIG_VIDEO additions
    #define ZAddons_Francesco_AdjustableFOV_VideoConfig \
    float fFOV;
     
    // ZConfiguration.h token and macro
    #define ZTOK_VIDEO_FOV "FOV"
    #define Z_VIDEO_FOV (ZGetConfiguration()->GetVideo()->fFOV)
     
    struct ZCONFIG_VIDEO; // forward declaration
     
    namespace ZAddons {
    namespace Francesco {
    namespace AdjustableFOV
    {
    void Init(ZCONFIG_VIDEO *cfg);
    bool TrySetFOV(float fDesiredFOV);
    bool TrySetConfiguredFOV();
    void SaveToFile(ZCONFIG_VIDEO *cfg, char *temp, MXmlElement *pVideoNode, MXmlElement *pNewElement);
    void LoadConfig(MXmlElement *pVideoNode, ZCONFIG_VIDEO *cfg);
    void InitCmds(ZChatCmdManager *cmdmgr);
    void InitInterfaceOption(ZIDLResource *pResource);
    void SaveInterfaceOption(ZIDLResource *pResource);
    }}}