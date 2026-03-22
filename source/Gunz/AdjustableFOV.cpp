    /*
      Addon Name: AdjustableFOV
    Revision: r1
    Author: Franc[e]sco
    */
     
    #include "stdafx.h"
     
    #include "AdjustableFOV.h"
    #include "ZConfiguration.h"
    #include "ZGame.h"
    #include "MDebug.h"
     
    void OutputCmdWrongArgument(const char *cmd); // kinda ugly way to do it
     
    namespace ZAddons {
    namespace Francesco {
    namespace AdjustableFOV
    {
     
    // tag for debug log
    const char *tag = "ZAddons::Francesco::AdjustableFOV";
     
    // addon settings
    const float fDefaultFOV = 90.f;
    const float fMinFOV = 70.f;
    const float fMaxFOV = 110.f;
     
    // textbox widget name in Interface\default\Options.xml
    const std::string strWidgetName("FOVEdit");
     
    void Init(ZCONFIG_VIDEO *cfg)
    {
    cfg->fFOV = fDefaultFOV;
    }
     
    bool TrySetFOV(float fDesiredFOV)
    {
    // reject fov's not allowed by the settings
    if (fDesiredFOV < fMinFOV || fDesiredFOV > fMaxFOV)
    {
    mlog("%s : attempted to set invalid FOV of %f\n", tag, fDesiredFOV);
    return false;
    }
     
    // note to self: make sure you don't mix up radians and degrees
    g_fFOV = D3DXToRadian(fDesiredFOV); // fov value used by the engine - in radians
    Z_VIDEO_FOV = fDesiredFOV; // fov value stored in ZConfiguration - in degrees
     
    return true;
    }
     
    // called when the game sets up the fov to start drawing the stage
    bool TrySetConfiguredFOV()
    {
    mlog("%s : applying current FOV of %f\n", tag, Z_VIDEO_FOV);
    return TrySetFOV(Z_VIDEO_FOV);
    }
     
    void SaveToFile(ZCONFIG_VIDEO *cfg, char *temp, MXmlElement *pVideoNode, MXmlElement *pNewElement)
    {
    // save current fov to config.xml
    pVideoNode->AppendText("\n\t\t");
    *pNewElement = pVideoNode->CreateChildElement(ZTOK_VIDEO_FOV);
    sprintf(temp, "%f", cfg->fFOV);
    pNewElement->SetContents(temp);
    // note: I'm using c-style string formatting just to reuse
    // the buffer in ZConfiguration::SaveToFile
    }
     
    void LoadConfig(MXmlElement *pVideoNode, ZCONFIG_VIDEO *cfg)
    {
    // load fov from config.xml
    pVideoNode->GetChildContents(&cfg->fFOV, ZTOK_VIDEO_FOV);
    }
     
    bool TrySetFOVFromString(std::string strFOV)
    {
    float fDesiredFOV = 0.f;
    std::istringstream iss;
     
    // correct the formatting to 123.45f or 123.f
    if (strFOV.find(".") == std::string::npos)
    strFOV.append(".");
     
    if (strFOV.find("f") == std::string::npos)
    strFOV.append("f");
     
    // finally convert the string to float
    iss.str(strFOV);
    iss >> fDesiredFOV;
     
    if (!iss.good())
    {
    mlog("%s : error parsing fov string, fDesiredFOV = %f\n", tag, fDesiredFOV);
    return false; // invalid fov value
    }
     
    if (!TrySetFOV(fDesiredFOV))
    return false; // fov out of range
     
    return true;
    }
     
    void ChatCmd_FOV(const char *line, const int argc, char ** const argv)
    {
    do
    {
    if (argc < 2)
    break;
     
    if (!TrySetFOVFromString(argv[1]))
    break;
     
    return; // fov successfully changed
    }
    while (false);
    // could also OR the two if's but it would look horrible
     
    // invalid syntax
    OutputCmdWrongArgument(argv[0]);
    }
     
    void InitCmds(ZChatCmdManager *cmdmgr)
    {
    // [todo] I could probabilly make a generic base class for commands to inherit from to save some code
    cmdmgr->AddCommand(0, "fov", &ChatCmd_FOV, CCF_GAME | CCF_STAGE | CCF_LOBBY, ARGVNoMin, 1, true, "/fov <70.0-110.0>", "changes the field of view");
    }
     
    MEdit *FindWidget(ZIDLResource *pResource)
    {
    // saves a little code since I'm obtaining the widget twice across the code
    return reinterpret_cast<MEdit*>(pResource->FindWidget(strWidgetName));
    }
     
    void InitInterfaceOption(ZIDLResource *pResource)
    {
    MEdit *pEdit = FindWidget(pResource);
     
    if (!pEdit)
    return;
     
    std::ostringstream oss;
     
    // load fov textbox value from config.xml
    oss << Z_VIDEO_FOV;
    pEdit->SetText(oss.str().c_str());
    pEdit->SetMaxLength(12);
    }
     
    void SaveInterfaceOption(ZIDLResource *pResource)
    {
    MEdit *pEdit = FindWidget(pResource);
     
    if (!pEdit)
    return;
     
    // set fov from the textbox value and save it to config.xml
    TrySetFOVFromString(pEdit->GetText());
    }
     
    }}}