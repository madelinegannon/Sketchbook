MODULE A079_EGM
    !***********************************************************************************
    !
    ! ETH Zurich / Robotic Fabrication Lab
    ! HIB C 13 / Stefano-Franscini-Platz 1
    ! CH-8093 Zurich
    !
    !***********************************************************************************
    !
    ! PROJECT     :  A079 Madeline
    !
    ! FUNCTION    :  EGM Template for Robot ROB_1
    !
    ! AUTHOR      :  Madeline Gannon
    !
    ! EMAIL       :  madeline@atonaton.com
    !
    ! HISTORY     :  2022.04.05
    !
    !***********************************************************************************

    !************************************************
    ! Function    :     EGM Template for Robot ROB_1 
    ! Programmer  :     Madeline Gannon
    ! Date        :     2022.04.05
    !***************** ETH Zurich *******************
    !
    VAR egmident egmID1;
    VAR egmstate egmSt1;
    CONST egm_minmax egm_minmax1:=[-0.001,0.001]; !Joint convergence: +/-0.1 degrees

    PROC run_egm()
        EGMReset egmID1;
        EGMGetId egmID1;
        egmSt1:=EGMGetState(egmID1);
        TPWrite "EGM state: "\Num:=egmSt1;
        IF egmSt1 <= EGM_STATE_CONNECTED THEN
            EGMSetupUC ROB_1, egmID1, "default", "EGM_11:" \Joint\ CommTimeout:=100;
            egmSt1:=EGMGetState(egmID1);
            IF egmSt1 = EGM_STATE_CONNECTED THEN
                TPWrite "ROB_1 CONNECTED!";
            ENDIF
        ENDIF 
        EGMActJoint egmID1, \J1:=egm_minmax1 \J2:=egm_minmax1 \J3:=egm_minmax1 \J4:=egm_minmax1 \J5:=egm_minmax1 \J6:=egm_minmax1 \J7:=egm_minmax1 \LpFilter:=3 \SampleRate:=24 \MaxSpeedDeviation:=100;
        EGMRunJoint egmID1, EGM_STOP_HOLD \J1 \J2 \J3 \J4 \J5 \J6 \J7 \CondTime:=20 \RampInTime:=2.0;
        
        egmSt1:=EGMGetState(egmID1);
        IF egmSt1=EGM_STATE_CONNECTED THEN
            TPWrite "Convergence condition fulfilled.. Resetting EGM.";
            EGMReset egmID1;
        ENDIF
    ENDPROC
ENDMODULE