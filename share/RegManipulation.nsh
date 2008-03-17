;----------------------------------------
; based upon a script of "Written by KiCHiK 2003-01-18 05:57:02"
; Update by Gauthier Quesnel 2008-03-13 17:00:00"
;          - Remove the 9x part
;          - Add environment variable
;----------------------------------------
!verbose 3
!include "WinMessages.NSH"
!verbose 4


;
; Determine admin versus local install by setting the SetShellVarContext
; using the type of the UserInfo account type.
;
Function un.onInit
  push $0
  push $1

  SetShellVarContext current
  ClearErrors
  UserInfo::GetName
  IfErrors done
  Pop $0
  UserInfo::GetAccountType
  Pop $1
  StrCmp $1 "Admin" 0 +3
    SetShellVarContext all
    Goto done
  StrCmp $1 "Power" 0 +3
    SetShellVarContext all
    Goto done
  done:
    Pop $1
    Pop $0
FunctionEnd

;
; Determine admin versus local install by setting the SetShellVarContext
; using the type of the UserInfo account type.
; Is install for "AllUsers" or "JustMe"?
; Default to "JustMe" - set to "AllUsers" if admin or on Win9x
; This function is used for the very first "custom page" of the installer.
; This custom page does not show up visibly, but it executes prior to the
; first visible page and sets up $INSTDIR properly...
; Choose different default installation folder based on SV_ALLUSERS...
; "Program Files" for AllUsers, "My Documents" for JustMe...
;
Function .onInit
  push $0
  push $1
  StrCpy $SV_ALLUSERS "JustMe"
  StrCpy $INSTDIR "$DOCUMENTS\${PRODUCT_INSTALL_DIRECTORY}"
  SetShellVarContext current

  ClearErrors
  UserInfo::GetName
  IfErrors noLM
  Pop $0
  UserInfo::GetAccountType
  Pop $1
  StrCmp $1 "Admin" 0 +4
    SetShellVarContext all
    StrCpy $SV_ALLUSERS "AllUsers"
    Goto done
  StrCmp $1 "Power" 0 +4
    SetShellVarContext all
    StrCpy $SV_ALLUSERS "AllUsers"
    Goto done
  noLM:
    StrCpy $SV_ALLUSERS "AllUsers"
  done:
    StrCmp $SV_ALLUSERS "AllUsers" 0 +2
    StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_INSTALL_DIRECTORY}"
    Pop $1
    Pop $0
FunctionEnd


;
; Determine admin versus local install by setting the SetShellVarContext
; using the type of the UserInfo account type.
; @Return all or current in the HEAD of stack.
;
!macro select_NT_profile UN
Function ${UN}select_NT_profile
  push $0
  push $1

  ClearErrors
  UserInfo::GetName
  IfErrors noLM
  Pop $0
  UserInfo::GetAccountType
  Pop $1
  StrCmp $1 "Admin" 0 +6
    SetShellVarContext all
    Pop $1
    Pop $0
    Push "all"
    Goto done 
  StrCmp $1 "Power" 0 +6
    SetShellVarContext all
    Pop $1
    Pop $0
    Push "all"
    Goto done 
  noLM:
    Pop $1
    Pop $0
    SetShellVarContext current
    Push "current"
    Goto done
  done:
FunctionEnd
!macroend


!insertmacro select_NT_profile ""
!insertmacro select_NT_profile "un."

!define NT_current_env 'HKCU "Environment"'
!define NT_all_env     'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
;
; IsNT - Returns 1 if the current system is NT, 0
;        otherwise.
;     Output: head of the stack
;
!macro IsNT UN
Function ${UN}IsNT
  Push $0
  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return
 
  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend


!insertmacro IsNT ""
!insertmacro IsNT "un."


;
; AddToPath - Adds the given dir to the search path.
;        Input - head of the stack
;        Note - Win9x systems requires reboot
;
Function AddToPath
   Exch $0
   Push $1
   Push $2
  
   Call IsNT
   Pop $1
   StrCmp $1 1 AddToPath_NT
      ; Not on NT
      MessageBox MB_OK "this message box will always show"
      Quit
 
   AddToPath_NT:
      Push $4
      Call select_NT_profile
      Pop  $4
 
      AddToPath_NT_selection_done:
      StrCmp $4 "current" read_path_NT_current
         ReadRegStr $1 ${NT_all_env} "PATH"
         Goto read_path_NT_resume
      read_path_NT_current:
         ReadRegStr $1 ${NT_current_env} "PATH"
      read_path_NT_resume:
      StrCpy $2 $0
      StrCmp $1 "" AddToPath_NTdoIt
         StrCpy $2 "$1;$0"
      AddToPath_NTdoIt:
         StrCmp $4 "current" write_path_NT_current
            ClearErrors
            WriteRegExpandStr ${NT_all_env} "PATH" $2
            IfErrors 0 write_path_NT_resume
            MessageBox MB_YESNO|MB_ICONQUESTION "The path could not be set for all users$\r$\nShould I try for the current user?" \
               IDNO write_path_NT_failed
            ; change selection
            StrCpy $4 "current"
            Goto AddToPath_NT_selection_done
         write_path_NT_current:
            ClearErrors
            WriteRegExpandStr ${NT_current_env} "PATH" $2
            IfErrors 0 write_path_NT_resume
            MessageBox MB_OK|MB_ICONINFORMATION "The path could not be set for the current user."
            Goto write_path_NT_failed
         write_path_NT_resume:
         SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
         DetailPrint "added path for user ($4), $0"
         write_path_NT_failed:
      
      Pop $4
   Pop $2
   Pop $1
   Pop $0
FunctionEnd

 
;
; RemoveFromPath - Remove a given dir from the path
;     Input: head of the stack
;
Function un.RemoveFromPath
   Exch $0
   Push $1
   Push $2
   Push $3
   Push $4
   
   Call un.IsNT
   Pop $1
   StrCmp $1 1 unRemoveFromPath_NT
      ; Not on NT
      MessageBox MB_OK "this message box will always show"
      Quit
 
   unRemoveFromPath_NT:
      StrLen $2 $0
      Call un.select_NT_profile
      Pop  $4
 
      StrCmp $4 "current" un_read_path_NT_current
         ReadRegStr $1 ${NT_all_env} "PATH"
         Goto un_read_path_NT_resume
      un_read_path_NT_current:
         ReadRegStr $1 ${NT_current_env} "PATH"
      un_read_path_NT_resume:
 
      Push $1
      Push $0
      Call un.StrStr ; Find $0 in $1
      Pop $0 ; pos of our dir
      IntCmp $0 -1 unRemoveFromPath_done
         ; else, it is in path
         StrCpy $3 $1 $0 ; $3 now has the part of the path before our dir
         IntOp $2 $2 + $0 ; $2 now contains the pos after our dir in the path (';')
         IntOp $2 $2 + 1 ; $2 now containts the pos after our dir and the semicolon.
         StrLen $0 $1
         StrCpy $1 $1 $0 $2
         StrCpy $3 "$3$1"
 
         StrCmp $4 "current" un_write_path_NT_current
            WriteRegExpandStr ${NT_all_env} "PATH" $3
	    DetailPrint "remove [$0] from PATH environment variable for user ($4)"
            Goto un_write_path_NT_resume
         un_write_path_NT_current:
            WriteRegExpandStr ${NT_current_env} "PATH" $3
	    DetailPrint "remove [$0] from PATH environment variable for user ($4)"
         un_write_path_NT_resume:
         SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
   unRemoveFromPath_done:
   Pop $4
   Pop $3
   Pop $2
   Pop $1
   Pop $0
FunctionEnd


;
; AddToEnvVar- Adds the given dir to the search path.
;        Input - head of the stack
;        Note - Win9x systems requires reboot
; push PKG_CONFIG_PATH
; push $INSTDIR/lib/pkgconfig
; call AddToEnvVar
;
Function AddToEnv
   Exch $1 ; $0 is the variable name
   Exch
   Exch $0 ; $1 is the variable value
   Push $2
   Push $3
   Push $4

   Call IsNT
   Pop $2
   StrCmp $2 1 AddToEnv_NT
      ; Not on NT
      MessageBox MB_OK "this message box will always show"
      Quit

   AddToEnv_NT:
      Push $4
      Call select_NT_profile
      Pop  $4

      AddToEnv_NT_selection_done:
      StrCmp $4 "current" read_env_NT_current
         ReadRegStr $2 ${NT_all_env} "$0"
         Goto read_env_NT_resume
      read_env_NT_current:
         ReadRegStr $2 ${NT_current_env} "$0"
      read_env_NT_resume:
      StrCpy $3 $1
      StrCmp $2 "" AddToEnv_NTdoIt
         StrCpy $3 "$2;$1"
      AddToEnv_NTdoIt:
         StrCmp $4 "current" write_env_NT_current
            ClearErrors
            WriteRegExpandStr ${NT_all_env} "$0" $3
            IfErrors 0 write_env_NT_resume
            MessageBox MB_YESNO|MB_ICONQUESTION "The environment variable $0 could not be set for all users$\r$\nShould I try for the current user?" \
               IDNO write_env_NT_failed
            ; change selection
            StrCpy $4 "current"
            Goto AddToEnv_NT_selection_done
         write_env_NT_current:
            ClearErrors
            WriteRegExpandStr ${NT_current_env} "$0" $3
            IfErrors 0 write_env_NT_resume
            MessageBox MB_OK|MB_ICONINFORMATION "The environment variable $0 could not be set for the current user."
            Goto write_env_NT_failed
         write_env_NT_resume:
         SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
         DetailPrint "added environment variable $0 for user ($4), $2"
         write_env_NT_failed:

   Pop $4
   Pop $3
   Pop $2
   Pop $1
   Pop $0
FunctionEnd


;
; RemoveFromPath - Remove a given dir from the path
;     Input: head of the stack
; push PKG_CONFIG_PATH
; push $INSTDIR/lib/pkgconfig
; call un.RemoveFromVar
;
Function un.RemoveFromEnv
   Exch $1 ; $1 is the variable name
   Exch
   Exch $0 ; $0 is the variable value
   Push $1
   Push $2
   Push $3
   Push $4
   Push $5
   Push $6
   Push $7
   Push $8

   Call un.IsNT
   Pop $2
   StrCmp $2 1 unRemoveFromEnv_NT
      ; Not on NT
      MessageBox MB_OK "this message box will always show"
      Quit

   unRemoveFromEnv_NT:
      StrLen $3 $1
      Call un.select_NT_profile
      Pop  $4

      StrCmp $4 "current" un_read_env_NT_current
         ReadRegStr $2 ${NT_all_env} "$0"
         Goto un_read_env_NT_resume
      un_read_env_NT_current:
         ReadRegStr $2 ${NT_current_env} "$0"
      un_read_env_NT_resume:

      Push $2
      Push $1
      Call un.StrStr ; Find $1 in $2
      Pop $5 ; pos of our dir
      IntCmp $5 -1 unRemoveFromEnv_done
         ; else, it is in env
         StrCpy $6 $2 $5 ; $6 now has the part of the env before our dir

         IntOp $7 $3 + $5 ; $7 now contains the pos after our dir in the env (';')
         IntOp $7 $7 + 1 ; $2 now containts the pos after our dir and the semicolon.

         StrLen $5 $2
         StrCpy $8 $2 $5 $7
         StrCpy $8 "$6$8"

         StrCmp $4 "current" un_write_env_NT_current
            WriteRegExpandStr ${NT_all_env} "$0" $8
	    DetailPrint "remove [$1] environment variable $0 for user ($4)"
            Goto un_write_env_NT_resume
         un_write_env_NT_current:
            WriteRegExpandStr ${NT_current_env} "$0" $8
	    DetailPrint "remove [$1] environment variable $0 for user ($4)"
         un_write_env_NT_resume:
         SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
   unRemoveFromEnv_done:
   Pop $8
   Pop $7
   Pop $6
   Pop $5
   Pop $4
   Pop $3
   Pop $2
   Pop $1
   Pop $0
FunctionEnd

;
; Macro for registering and unregistering file extensions in NSIS scripts.
; !include "registerExtension.nsh"
; ...
; # later, inside a section:
; ${registerExtension} "c:\myplayer.exe" ".mkv" "MKV File"
;  
; ${unregisterExtension} ".mkv" "MKV File"

!define registerExtension "!insertmacro registerExtension"
!define unregisterExtension "!insertmacro unregisterExtension"
 
!macro registerExtension executable extension description
       Push "${executable}"  ; "full path to my.exe"
       Push "${extension}"   ;  ".mkv"
       Push "${description}" ;  "MKV File"
       Call registerExtension
!macroend
 
; back up old value of .opt
Function registerExtension
!define Index "Line${__LINE__}"
  pop $R0 ; ext name
  pop $R1
  pop $R2
  push $1
  push $0
  ReadRegStr $1 SHCTX $R1 ""
  StrCmp $1 "" "${Index}-NoBackup"
    StrCmp $1 "OptionsFile" "${Index}-NoBackup"
    WriteRegStr SHCTX $R1 "backup_val" $1
"${Index}-NoBackup:"
  WriteRegStr SHCTX $R1 "" $R0
  ReadRegStr $0 SHCTX $R0 ""
  StrCmp $0 "" 0 "${Index}-Skip"
	WriteRegStr SHCTX $R0 "" $R0
	WriteRegStr SHCTX "$R0\shell" "" "open"
	WriteRegStr SHCTX "$R0\DefaultIcon" "" "$R2,0"
"${Index}-Skip:"
  WriteRegStr SHCTX "$R0\shell\open\command" "" '$R2 "%1"'
  WriteRegStr SHCTX "$R0\shell\edit" "" "Edit $R0"
  WriteRegStr SHCTX "$R0\shell\edit\command" "" '$R2 "%1"'
  pop $0
  pop $1
!undef Index
FunctionEnd
 
!macro unregisterExtension extension description
       Push "${extension}"   ;  ".vpz"
       Push "${description}"   ;  "VLE Project file"
       Call un.unregisterExtension
!macroend
 
Function un.unregisterExtension
  pop $R1 ; description
  pop $R0 ; extension
!define Index "Line${__LINE__}"
  ReadRegStr $1 SHCTX $R0 ""
  StrCmp $1 $R1 0 "${Index}-NoOwn" ; only do this if we own it
  ReadRegStr $1 SHCTX $R0 "backup_val"
  StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
  DeleteRegKey SHCTX $R0
  Goto "${Index}-NoOwn"
"${Index}-Restore:"
  WriteRegStr SHCTX $R0 "" $1
  DeleteRegValue SHCTX $R0 "backup_val"
  DeleteRegKey SHCTX $R1 ;Delete key with association name settings
"${Index}-NoOwn:"
!undef Index
FunctionEnd



;
; StrStr - Finds a given string in another given string.
;               Returns -1 if not found and the pos if found.
;          Input: head of the stack - string to find
;                      second in the stack - string to find in
;          Output: head of the stack
;
Function un.StrStr
  Push $0
  Exch
  Pop $0 ; $0 now have the string to find
  Push $1
  Exch 2
  Pop $1 ; $1 now have the string to find in
  Exch
  Push $2
  Push $3
  Push $4
  Push $5
 
  StrCpy $2 -1
  StrLen $3 $0
  StrLen $4 $1
  IntOp $4 $4 - $3
 
  unStrStr_loop:
    IntOp $2 $2 + 1
    IntCmp $2 $4 0 0 unStrStrReturn_notFound
    StrCpy $5 $1 $3 $2
    StrCmp $5 $0 unStrStr_done unStrStr_loop
 
  unStrStrReturn_notFound:
    StrCpy $2 -1
 
  unStrStr_done:
    Pop $5
    Pop $4
    Pop $3
    Exch $2
    Exch 2
    Pop $0
    Pop $1
FunctionEnd
