/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2013                                                    *
 * Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China            *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the      *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL       *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR       *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#ifndef __Player_H__
#define __Player_H__

#include "Client.h"
#include "DecisionTree.h"
#include "DynamicDebug.h"
#include "Formation.h"
#include "CommandSender.h"
#include "Parser.h"
#include "Thread.h"
#include "UDPSocket.h"
#include "WorldModel.h"
#include "Agent.h"
#include "VisualSystem.h"
#include "Logger.h"
#include "CommunicateSystem.h"
#include "TimeTest.h"
#include "Dasher.h"
#include "Kicker.h"
#include "Tackler.h"
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <string>
#include <cstring> 

class DecisionTree;
class  BeliefState;

class Player: public Client
{
	//DecisionTree *mpDecisionTree; //hereo

public:
    /**
     * 构造函数和析构函数
     */
    bool isPositioned;
    bool mpIntransit = false;
    Vector mpTarget = Vector(0,0); 

    Player();
    virtual ~Player();

    void Run();
    void SendOptionToServer();

    bool PassPlayersAvailable(){
    	Vector myPosition = mpAgent->GetSelf().GetPos();
    	Vector currentHole = RoundToNearestHole(myPosition);
    	Vector frontup = Vector(currentHole.X()+10, currentHole.Y()-10);
    	Vector backup = Vector(currentHole.X()-10, currentHole.Y()-10);
    	Vector frontdown = Vector(currentHole.X()+10, currentHole.Y()+10);
    	Vector backdown = Vector(currentHole.X()-10, currentHole.Y()+10);

        Vector fronthor = Vector(currentHole.X()+20, currentHole.Y());
        Vector backhor = Vector(currentHole.X()-20, currentHole.Y());
        Vector upvert = Vector(currentHole.X(), currentHole.Y()-20);
        Vector downvert = Vector(currentHole.X(), currentHole.Y()+20);
    	
    	double buffer = 0.5;
    	
    	//TODO: Can be replaced by the IsOccupied function
        //TODO: Return true only if pass is advantageous
    	
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if( AreSamePoints(player_pos, frontup, buffer)||
    		    AreSamePoints(player_pos, frontdown, buffer)||
                //AreSamePoints(player_pos, backup, buffer)||
                //AreSamePoints(player_pos, backdown, buffer)||
                AreSamePoints(player_pos, fronthor, buffer)||
                //AreSamePoints(player_pos, backhor, buffer)||
                AreSamePoints(player_pos, upvert, buffer)||
                AreSamePoints(player_pos, downvert, buffer)
    			){
    			std::cout<<"pass available"<<std::endl;
    			return true;
    		}
    	}
    	return false;	
    }

    bool IsOccupied(Vector target){
    	//Returns true if target is occupied by a player
    	double buffer = 0.3;
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, target, buffer)&&i!=mpAgent->GetSelfUnum())
    			return true;
    	}
    	return false;
    }



    Unum GetClosest(Vector target){
    	//Excluded ballholder from the below equation
    	double mindis = 999;
    	Unum mindisUnum = 99;
    	Unum BHUnum = GetBHUnum();
    	if(BHUnum==-1)
    		std::cout<<"No ball holder"<<std::endl;
    	for(Unum i=1; i<=11; i++){
    		double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
    		if(PlayerDis<=mindis&&i!=BHUnum){
    			mindis = PlayerDis;
    			mindisUnum = i;
    		}
    	}

        return mindisUnum;
    	//std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
    	/*
        if(mpAgent->GetSelfUnum()==mindisUnum)
    		return true;
    	else 
    		return false;
            */
    }

    Unum GetClosestExcl1(Vector target, Unum ex1){
        //Excluded ballholder and ex1 from the below equation
        //ex1 would already have been assigned to another hole
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
        //std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
        /*
        if(mpAgent->GetSelfUnum()==mindisUnum)
            return true;
        else 
            return false;
            */
    }

    Unum GetClosestExcl2(Vector target, Unum ex1, Unum ex2){
        //Excluded ballholder and ex1,2 from the below equation
        //ex1,2 would already have been assigned to another hole
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
       return mindisUnum;
        //std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
        /*
        if(mpAgent->GetSelfUnum()==mindisUnum)
            return true;
        else 
            return false;
            */
    }

    Unum GetClosestExcl3(Vector target, Unum ex1, Unum ex2, Unum ex3){
        //Excluded ballholder and ex1,2,3 from the below equation
        //ex1,2,3 would already have been assigned to another hole
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
        //std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
        /*
        if(mpAgent->GetSelfUnum()==mindisUnum)
            return true;
        else 
            return false;
            */
    }

    Unum GetClosestExcl4(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestExcl5(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4, Unum ex5){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestExcl6(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4, Unum ex5, Unum ex6){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5&&i!=ex6){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestExcl7(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4, Unum ex5, Unum ex6, Unum ex7){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5&&i!=ex6&&i!=ex7){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }


    void DecideAndOccupyHole(){
    	//Called when another teammate has the ball
    	//Decide if the player should support the ballholder by moving to an appropriate hole or not
    	//Act as per the decision
    	
    	Vector BHPos;
    	
    	double buffer = 0.1;
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable()&&(i!=mpAgent->GetSelfUnum())){
    			BHPos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    			break;
    		}
    	}
    	BHPos = RoundToNearestHole(BHPos);

    	Vector BHfrontup = Vector(BHPos.X()+10, BHPos.Y()-10);
    	Vector BHbackup = Vector(BHPos.X()-10, BHPos.Y()-10);
    	Vector BHfrontdown = Vector(BHPos.X()+10, BHPos.Y()+10);
    	Vector BHbackdown = Vector(BHPos.X()-10, BHPos.Y()+10);

        Vector BHfronthor = Vector(BHPos.X()+20, BHPos.Y());
        Vector BHbackhor = Vector(BHPos.X()-20, BHPos.Y());
        Vector BHupvert = Vector(BHPos.X(), BHPos.Y()-20);
        Vector BHdownvert = Vector(BHPos.X(), BHPos.Y()+20);
    	//if frontup&frontdown not occupied (+ other conditions), move there
    	//TODO: Currently, it is possible that one player is expected to fill both the holes

        Unum FU, FD, BU, BD;
        Unum FH, BH, UV, DV;

        //&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25

        FU = GetClosest(BHfrontup);
        if(FU==mpAgent->GetSelfUnum()&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25){
            OccupyHole(BHfrontup);
            std::cout<<"Player "<<FU<<" occupying frontup"<<std::endl;
        }

        FD = GetClosestExcl1(BHfrontdown, FU);
        if(FD==mpAgent->GetSelfUnum()&&BHfrontdown.X()>=-45&&BHfrontdown.X()<=45&&BHfrontdown.Y()>=-25&&BHfrontdown.Y()<=25){
            OccupyHole(BHfrontdown);
            std::cout<<"Player "<<FD<<" occupying frontdown"<<std::endl;
        }

        FH = GetClosestExcl2(BHfronthor, FU, FD);
        if(FH==mpAgent->GetSelfUnum()&&BHfronthor.X()>=-45&&BHfronthor.X()<=45&&BHfronthor.Y()>=-25&&BHfronthor.Y()<=25){
            OccupyHole(BHfronthor);
            std::cout<<"Player "<<FH<<" occupying backdown"<<std::endl;
        }

        UV = GetClosestExcl3(BHupvert, FU, FD, FH);
        if(UV==mpAgent->GetSelfUnum()&&BHupvert.X()>=-45&&BHupvert.X()<=45&&BHupvert.Y()>=-25&&BHupvert.Y()<=25){
            OccupyHole(BHupvert);
            std::cout<<"Player "<<UV<<" occupying backdown"<<std::endl;
        }

        DV = GetClosestExcl4(BHdownvert, FU, FD, FH, UV);
        if(DV==mpAgent->GetSelfUnum()&&BHdownvert.X()>=-45&&BHdownvert.X()<=45&&BHdownvert.Y()>=-25&&BHdownvert.Y()<=25){
            OccupyHole(BHdownvert);
            std::cout<<"Player "<<DV<<" occupying backdown"<<std::endl;
        }

        BU = GetClosestExcl5(BHbackup, FU, FD, FH, UV, DV);
        if(BU==mpAgent->GetSelfUnum()&&BHbackup.X()>=-45&&BHbackup.X()<=45&&BHbackup.Y()>=-25&&BHbackup.Y()<=25){
            OccupyHole(BHbackup);
            std::cout<<"Player "<<BU<<" occupying backup"<<std::endl;
        }
        
        BD = GetClosestExcl6(BHbackdown, FU, FD, FH, UV, DV, BU);
        if(BD==mpAgent->GetSelfUnum()&&BHbackdown.X()>=-45&&BHbackdown.X()<=45&&BHbackdown.Y()>=-25&&BHbackdown.Y()<=25){
            OccupyHole(BHbackdown);
            std::cout<<"Player "<<BD<<" occupying backdown"<<std::endl;
        }


        BH = GetClosestExcl7(BHbackhor, FU, FD, FH, UV, DV, BU, BD);
        if(BH==mpAgent->GetSelfUnum()&&BHbackhor.X()>=-45&&BHbackhor.X()<=45&&BHbackhor.Y()>=-25&&BHbackhor.Y()<=25){
            OccupyHole(BHbackhor);
            std::cout<<"Player "<<BH<<" occupying backdown"<<std::endl;
        }



        /*
    	if(!IsOccupied(BHfrontup)||!IsOccupied(BHfrontdown)){
    		//&&mpAgent->GetSelfUnum()!=10
    		//if closest to frontup/frontdown, occupy it
    		if(IsClosest(BHfrontup)&&!IsOccupied(BHfrontup)){
                OccupyHole(RoundToNearestHole(BHfrontup));
                return;
            }
    		else if(IsClosest(BHfrontdown)&&!IsOccupied(BHfrontdown)){
    			OccupyHole(RoundToNearestHole(BHfrontdown));
                return;
            }
    	}
    	
        if(!IsOccupied(BHbackup)||!IsOccupied(BHbackdown)){
            //&&mpAgent->GetSelfUnum()==10    
    		//if closest to frontup/frontdown, occupy it
    		if(IsClosest(BHbackup)&&!IsOccupied(BHbackup)){
    			OccupyHole(RoundToNearestHole(BHbackup));
                return;
            }
    		else if(IsClosest(BHbackdown)&&!IsOccupied(BHbackdown)){
    			OccupyHole(RoundToNearestHole(BHbackdown));
                return;
            }
    	}
        */
    }

    void DecideAndOccupyHole(Unum target){
        //Called when another teammate has the ball
        //Decide if the player should support the ballholder by moving to an appropriate hole or not
        //Act as per the decision
        
        Vector BHPos;
        
        double buffer = 0.3;
        
        if(target!=mpAgent->GetSelfUnum()){
            BHPos = mpAgent->GetWorldState().GetTeammate(target).GetPos();
        }
        else
            return;
        
        BHPos = RoundToNearestHole(BHPos);

        Vector BHfrontup = Vector(BHPos.X()+10, BHPos.Y()-10);
        Vector BHbackup = Vector(BHPos.X()-10, BHPos.Y()-10);
        Vector BHfrontdown = Vector(BHPos.X()+10, BHPos.Y()+10);
        Vector BHbackdown = Vector(BHPos.X()-10, BHPos.Y()+10);

        Vector BHfronthor = Vector(BHPos.X()+20, BHPos.Y());
        Vector BHbackhor = Vector(BHPos.X()-20, BHPos.Y());
        Vector BHupvert = Vector(BHPos.X(), BHPos.Y()-20);
        Vector BHdownvert = Vector(BHPos.X(), BHPos.Y()+20);
        //if frontup&frontdown not occupied (+ other conditions), move there
        //TODO: Currently, it is possible that one player is expected to fill both the holes

        Unum FU, FD, BU, BD;
        Unum FH, BH, UV, DV;

        //&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25

        FU = GetClosest(BHfrontup);
        if(FU==mpAgent->GetSelfUnum()&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25){
            OccupyHole(BHfrontup);
            std::cout<<"Player "<<FU<<" occupying frontup"<<std::endl;
        }

        FD = GetClosestExcl1(BHfrontdown, FU);
        if(FD==mpAgent->GetSelfUnum()&&BHfrontdown.X()>=-45&&BHfrontdown.X()<=45&&BHfrontdown.Y()>=-25&&BHfrontdown.Y()<=25){
            OccupyHole(BHfrontdown);
            std::cout<<"Player "<<FD<<" occupying frontdown"<<std::endl;
        }

        FH = GetClosestExcl2(BHfronthor, FU, FD);
        if(FH==mpAgent->GetSelfUnum()&&BHfronthor.X()>=-45&&BHfronthor.X()<=45&&BHfronthor.Y()>=-25&&BHfronthor.Y()<=25){
            OccupyHole(BHfronthor);
            std::cout<<"Player "<<FH<<" occupying backdown"<<std::endl;
        }

        UV = GetClosestExcl3(BHupvert, FU, FD, FH);
        if(UV==mpAgent->GetSelfUnum()&&BHupvert.X()>=-45&&BHupvert.X()<=45&&BHupvert.Y()>=-25&&BHupvert.Y()<=25){
            OccupyHole(BHupvert);
            std::cout<<"Player "<<UV<<" occupying backdown"<<std::endl;
        }

        DV = GetClosestExcl4(BHdownvert, FU, FD, FH, UV);
        if(DV==mpAgent->GetSelfUnum()&&BHdownvert.X()>=-45&&BHdownvert.X()<=45&&BHdownvert.Y()>=-25&&BHdownvert.Y()<=25){
            OccupyHole(BHdownvert);
            std::cout<<"Player "<<DV<<" occupying backdown"<<std::endl;
        }

        BU = GetClosestExcl5(BHbackup, FU, FD, FH, UV, DV);
        if(BU==mpAgent->GetSelfUnum()&&BHbackup.X()>=-45&&BHbackup.X()<=45&&BHbackup.Y()>=-25&&BHbackup.Y()<=25){
            OccupyHole(BHbackup);
            std::cout<<"Player "<<BU<<" occupying backup"<<std::endl;
        }
        
        BD = GetClosestExcl6(BHbackdown, FU, FD, FH, UV, DV, BU);
        if(BD==mpAgent->GetSelfUnum()&&BHbackdown.X()>=-45&&BHbackdown.X()<=45&&BHbackdown.Y()>=-25&&BHbackdown.Y()<=25){
            OccupyHole(BHbackdown);
            std::cout<<"Player "<<BD<<" occupying backdown"<<std::endl;
        }


        BH = GetClosestExcl7(BHbackhor, FU, FD, FH, UV, DV, BU, BD);
        if(BH==mpAgent->GetSelfUnum()&&BHbackhor.X()>=-45&&BHbackhor.X()<=45&&BHbackhor.Y()>=-25&&BHbackhor.Y()<=25){
            OccupyHole(BHbackhor);
            std::cout<<"Player "<<BH<<" occupying backdown"<<std::endl;
        }



        /*
        if(!IsOccupied(BHfrontup)||!IsOccupied(BHfrontdown)){
            //&&mpAgent->GetSelfUnum()!=10
            //if closest to frontup/frontdown, occupy it
            if(IsClosest(BHfrontup)&&!IsOccupied(BHfrontup)){
                OccupyHole(RoundToNearestHole(BHfrontup));
                return;
            }
            else if(IsClosest(BHfrontdown)&&!IsOccupied(BHfrontdown)){
                OccupyHole(RoundToNearestHole(BHfrontdown));
                return;
            }
        }
        
        if(!IsOccupied(BHbackup)||!IsOccupied(BHbackdown)){
            //&&mpAgent->GetSelfUnum()==10    
            //if closest to frontup/frontdown, occupy it
            if(IsClosest(BHbackup)&&!IsOccupied(BHbackup)){
                OccupyHole(RoundToNearestHole(BHbackup));
                return;
            }
            else if(IsClosest(BHbackdown)&&!IsOccupied(BHbackdown)){
                OccupyHole(RoundToNearestHole(BHbackdown));
                return;
            }
        }
        */
    }

    bool BallKickableByATeammate(){
    	//TODO: Replace IsKickable with AreSamePoints + larger buffer
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable()&&(i!=mpAgent->GetSelfUnum())){
    			//std::cout<<"Player "<<mpAgent->GetSelfUnum()<<" thinks ball kickable by Player "<<i<<std::endl;
    			return true;
    		}
    	}
    	return false;
    }

    Unum GetBHUnum(){
    	//Only to be called if BallKickableByATeammate
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable())
    			return i;
    	}
    	return -1;
    }


    void PlaceThePlayers(){
        Vector myPosition = mpAgent->GetSelf().GetPos();

        Vector currentHole = RoundToNearestHole(myPosition);


        for(Unum i=1;i<=11;i++){

            
        }

    }

    bool PassToBestPlayer(){
        //TODO: Use transit variable for faster calling of the OccupyHole/Dasher functions
        Vector myPosition = mpAgent->GetSelf().GetPos();
        Vector currentHole = RoundToNearestHole(myPosition);

        Vector frontup = Vector(currentHole.X()+10, currentHole.Y()-10);
        Vector backup = Vector(currentHole.X()-10, currentHole.Y()-10);
        Vector frontdown = Vector(currentHole.X()+10, currentHole.Y()+10);
        Vector backdown = Vector(currentHole.X()-10, currentHole.Y()+10);

        Vector fronthor = Vector(currentHole.X()+20, currentHole.Y());
        Vector backhor = Vector(currentHole.X()-20, currentHole.Y());
        Vector upvert = Vector(currentHole.X(), currentHole.Y()-20);
        Vector downvert = Vector(currentHole.X(), currentHole.Y()+20);


        double frontup_threat =0.0;
        double backup_threat =0.0;
        double frontdown_threat =0.0;
        double backdown_threat =0.0;
        double min_threat = 10000000.0;

        double fronthor_threat = 0.0;
        double backhor_threat = 0.0;
        double upvert_threat = 0.0;
        double downvert_threat = 0.0;
        std::vector <std::pair<std::string,double> > max_threat;    
        
        double buffer = 0.5;
        double f = 0.6;


//A little change
std::cout<<"Hello I'm in PassToBestPlayer"<<std::endl;
/*for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, frontup, buffer)){
                Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
                return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, frontdown, buffer)){
                Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
                return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backup, buffer)){
                Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
                return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backdown, buffer)){
                Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
                return;
            }
        }




 */

 //     

 for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            //std::cout<<"I'm in first for loop"<<std::endl;

        double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if((myPosition.X() < mpAgent->GetWorldState().GetOpponent(i).GetPos().X()) && mpAgent->GetWorldState().GetOpponent(i).GetPos().Y() >= 0.0 ) {

            double xhole = frontup.X();
            double yhole = frontup.Y();

            std::cout<<"xhole:"<<xhole<<std::endl;

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

             std::cout<<"mpposx:"<<mpposx<<std::endl;

            double slope = ((xhole - mpposx)/(yhole - mpposy));

             std::cout<<"slope:"<<slope<<std::endl;

            double ccon = yhole -(slope*xhole);

             std::cout<<"ccon:"<<ccon<<std::endl;

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));

            std::cout<<"shortest_dist :"<<shortest_dist<<std::endl;

            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                frontup_threat += shortest_dist - dist_to_perpendicular; 
                 std::cout<<"frontup:"<<frontup_threat<<std::endl;
                 max_threat.push_back(std::make_pair("frontup_threat",abs(frontup_threat)));
            }
            }
        }
        }  


        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            //std::cout<<"I'm in first for loop"<<std::endl;

        double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if((myPosition.X() > mpAgent->GetWorldState().GetOpponent(i).GetPos().X()) && mpAgent->GetWorldState().GetOpponent(i).GetPos().Y() >= 0.0 ) {

            double xhole = backup.X();
            double yhole = backup.Y();

            std::cout<<"xhole:"<<xhole<<std::endl;

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

             std::cout<<"mpposx:"<<mpposx<<std::endl;

            double slope = ((xhole - mpposx)/(yhole - mpposy));

             std::cout<<"slope:"<<slope<<std::endl;

            double ccon = yhole -(slope*xhole);

             std::cout<<"ccon:"<<ccon<<std::endl;

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));

            std::cout<<"shortest_dist :"<<shortest_dist<<std::endl;

            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                backup_threat += shortest_dist - dist_to_perpendicular; 
                 std::cout<<"backup:"<<backup_threat<<std::endl;
                 max_threat.push_back(std::make_pair("backup_threat",abs(backup_threat)));
            }
            }
        }
        }  

        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            //std::cout<<"I'm in first for loop"<<std::endl;

        double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if((myPosition.X() > mpAgent->GetWorldState().GetOpponent(i).GetPos().X()) && mpAgent->GetWorldState().GetOpponent(i).GetPos().Y() <= 0.0 ) {

            double xhole = backdown.X();
            double yhole = backdown.Y();

            std::cout<<"xhole:"<<xhole<<std::endl;

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

             std::cout<<"mpposx:"<<mpposx<<std::endl;

            double slope = ((xhole - mpposx)/(yhole - mpposy));

             std::cout<<"slope:"<<slope<<std::endl;

            double ccon = yhole -(slope*xhole);

             std::cout<<"ccon:"<<ccon<<std::endl;

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));

            std::cout<<"shortest_dist :"<<shortest_dist<<std::endl;

            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                backdown_threat += shortest_dist - dist_to_perpendicular; 
                 std::cout<<"backdown:"<<backdown_threat<<std::endl;
                 max_threat.push_back(std::make_pair("backdown_threat",abs(backdown_threat)));
            }
            }
        }
        }  


        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            //std::cout<<"I'm in first for loop"<<std::endl;

        double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if((myPosition.X() < mpAgent->GetWorldState().GetOpponent(i).GetPos().X()) && mpAgent->GetWorldState().GetOpponent(i).GetPos().Y() >= 0.0 ) {

            double xhole = frontdown.X();
            double yhole = frontdown.Y();

            std::cout<<"xhole:"<<xhole<<std::endl;

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

             std::cout<<"mpposx:"<<mpposx<<std::endl;

            double slope = ((xhole - mpposx)/(yhole - mpposy));

             std::cout<<"slope:"<<slope<<std::endl;

            double ccon = yhole -(slope*xhole);

             std::cout<<"ccon:"<<ccon<<std::endl;

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));

            std::cout<<"shortest_dist :"<<shortest_dist<<std::endl;

            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                frontdown_threat += shortest_dist - dist_to_perpendicular; 
                 std::cout<<"frontdown:"<<frontdown_threat<<std::endl;
                 max_threat.push_back(std::make_pair("frontdown_threat",abs(frontdown_threat)));
            }
            }
        }
        }  





        
        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            //std::cout<<"I'm in first for loop"<<std::endl;

        double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if(myPosition.X() < mpAgent->GetWorldState().GetOpponent(i).GetPos().X()){

            double xhole = fronthor.X();
            double yhole = fronthor.Y();

            std::cout<<"xhole:"<<xhole<<std::endl;

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

             std::cout<<"mpposx:"<<mpposx<<std::endl;

            double slope = ((xhole - mpposx)/(yhole - mpposy));

             std::cout<<"slope:"<<slope<<std::endl;

            double ccon = yhole -(slope*xhole);

             std::cout<<"ccon:"<<ccon<<std::endl;

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));

            std::cout<<"shortest_dist :"<<shortest_dist<<std::endl;

            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                fronthor_threat += shortest_dist - dist_to_perpendicular; 
                 std::cout<<"fronthor:"<<fronthor_threat<<std::endl;
                 max_threat.push_back(std::make_pair("fronthor_threat",abs(fronthor_threat)));
            }
            }
        }
        }



        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if(myPosition.Y() < mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()){

            double xhole = upvert.X();
            double yhole = upvert.Y();

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

            double slope = ((xhole - mpposx)/(yhole - mpposy));

            double ccon = yhole -(slope*xhole);

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));


            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                upvert_threat += shortest_dist - dist_to_perpendicular; 
                max_threat.push_back(std::make_pair("upvert_threat",abs(upvert_threat)));
            }
        }
        }
    }
    



        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if(myPosition.X() > mpAgent->GetWorldState().GetOpponent(i).GetPos().X()){

            double xhole = backhor.X();
            double yhole = backhor.Y();

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

            double slope = ((xhole - mpposx)/(yhole - mpposy));

            double ccon = yhole -(slope*xhole);

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));


            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                backhor_threat += shortest_dist - dist_to_perpendicular; 
                max_threat.push_back(std::make_pair("backhor_threat",abs(backhor_threat)));
            }
            }
        }
        }


        for(Unum i=1;i<=11;i++){
            //Vector player_opp = mpAgent->GetWorldState().mpAgent->GetWorldState().GetOpponent(i).GetPos();

            double ball_holder_to_opp_dis = (mpAgent->GetWorldState().GetOpponent(i).GetPos() - myPosition).Mod();
         if(ball_holder_to_opp_dis <=20.0){  

            if(myPosition.Y() > mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()){

            double xhole = downvert.X();
            double yhole = downvert.Y();

            double mpposx = myPosition.X(); 
            double mpposy = myPosition.Y();

            double slope = ((xhole - mpposx)/(yhole - mpposy));

            double ccon = yhole -(slope*xhole);

            double shortest_dist = abs((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()*slope)+(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()*1)+(ccon))/sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X(),2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y(),2));


            double px = xhole - mpposx;
            double py = yhole - mpposy;

            double u = (((mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-mpposx)*px )+((mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-mpposy)*py))/((px*px)+(py*py));

            double currposx = mpposx + u;
            double currposy = mpposy + u;

            double dist_to_perpendicular = sqrt(pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().X()-currposx,2)+pow(mpAgent->GetWorldState().GetOpponent(i).GetPos().Y()-currposy,2));

            if((dist_to_perpendicular/100.0)>(shortest_dist/100.0)) {
                downvert_threat += shortest_dist - dist_to_perpendicular; 
                max_threat.push_back(std::make_pair("downvert_threat",abs(downvert_threat)));
            }
            }
        }
        }

        //MAX Threat


    std::cout<<"Calculating Threat"<<std::endl;
    std::sort (max_threat.begin(), max_threat.end());
    std::cout<<"After sort"<<std::endl;
    char max_t ='D';
    std::cout<<"After Max_t"<<std::endl;
    char * str="hello";
    std::cout<<"After hello"<<std::endl;
    std::cout<<"After const c"<<std::endl;    
    std::cout<<"The string"<<str<<std::endl;
    double max_thr=0.0;
    char * max_thr_str = new char[50];

    

    if(!max_threat.empty()){
    std::cout<<"Hello i'm in if conditions"<<std::endl;
        for(int i = 0; i < max_threat.size (); i++)
    {
            std::cout<<"Hello i'm in for loop"<<std::endl;
        if(max_threat[i].second > max_thr){
            std::cout<<"Hello i'm in second if conditions"<<std::endl;
            max_thr=max_threat[i].second;
            std::cout<<"Hello i'm in max_thr conditions"<<max_thr<<std::endl;
            std::cout<<"max str"<<max_threat[i].first.c_str()<<std::endl;

            /*for(int i=0;i<strlen(max_threat[i].first.c_str());i++){
                std::cout<<max_threat[i].first.c_str()[i];
            }*/
            std::cout<<"type of str :"<<typeid(str).name()<<std::endl;
            std::cout<<"type of str :"<<typeid(max_threat[i].first.c_str()).name()<<std::endl;
            std::strcpy(max_thr_str,max_threat[i].first.c_str());

            std::cout<<"Hello i'm after strcpy conditions"<<std::endl;
           // const char* max_thr_str = max_threat[i].second.c_str();
        }

        std::cout << max_threat[i].first << ":" << max_threat[i].second<< std::endl; 
    }

        //std::cout<<"The max value"<<max_threat.end().first<<std::endl;
        //std::cout<<"The max string"<<max_threat.end().second<<std::endl;
       
         //strcpy(str,max_thr_str);
         std::cout<<"After strcpy"<<std::endl;

        

        if(std::strcmp(max_thr_str,"backdown_threat")==0){
            max_t = 'A';
        }

        else if(std::strcmp(max_thr_str,"backup_threat")==0){
            max_t='B';
        }

        else if(std::strcmp(max_thr_str,"frontup_threat")==0){
            max_t='C';
        }

        else if(std::strcmp(max_thr_str,"frontdown_threat")==0){
            max_t='D';
        }


        else if(std::strcmp(max_thr_str,"fronthor_threat")==0){
            max_t='E';
        }

        else if(std::strcmp(max_thr_str,"backhor_threat")==0){
            max_t='F';
        }
        else if(std::strcmp(max_thr_str,"upvert_threat")==0){
            max_t='G';
        }

        else if(std::strcmp(max_thr_str,"downvert_threat")==0){
            max_t='H';
        }
        else{

        }

    }

    else{

            max_t = 'I';
            std::cout<<"Hello lag gayi"<<std::endl;
        }

        switch(max_t){

            case 'A':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos, backdown, buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                    std::cout<<"I kicked towards backdown"<<std::endl;
                                    return Kicker::instance().KickBall(*mpAgent, backdown, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);   
                            }
                            else{
                                continue;
                            }

                        }
                                
            case 'B':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                                if(AreSamePoints(player_pos,backup,buffer)){
                                    std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards backup"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, backup, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);   
                        
                            }
                                else{
                                    continue;
                                }

                        }
                                
            case 'C':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,frontup,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards frontup"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, frontup, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                                
                            }
                            else{
                                continue;
                            }

                        }
                                
            case 'D':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,frontdown,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards frontdown"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, frontdown, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                                
                            }
                            else{
                               continue;
                            }

                        }
                        break;

            case 'E':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,fronthor,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards fronthor"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, fronthor, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                            
                            }
                            else{
                               continue;
                            }

                        }
                        break;

                case 'F':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,backhor,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards backhor"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, backhor, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                                
                            }
                            else{
                               continue;
                            }
                        }
                        break;

                case 'G':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,upvert,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards upvert"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, upvert, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                                
                            }
                            else{
                               continue;
                            }

                        }
                        break;

                case 'H':
                        for(Unum i=1;i<=11;i++){
                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,downvert,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"I kicked towards downvert"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, downvert, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                                
                            }
                            else{
                               continue;
                            }

                        }
                        break;
                case 'I':
                       for(Unum i=1;i<=11;i++){

                            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                            if(AreSamePoints(player_pos,fronthor,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"passing to fronthor"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, fronthor, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                            }
                            else if(AreSamePoints(player_pos,frontup,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"passing to frontup"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, frontup, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                                
                             }
                            else if(AreSamePoints(player_pos,frontdown,buffer)){
                                std::string msgstr = "cusp";
                                Unum mynum = mpAgent->GetSelfUnum();
                                std::string result;
                                std::stringstream sstm;
                                sstm << msgstr << mynum <<"X"<< i;
                                result.append(sstm.str());
                                std::cout<<"saying - "<<result<<std::endl;
                                while(!(mpAgent->Say(result)));
                                std::cout<<"passing to frontdown"<<std::endl;
                                return Kicker::instance().KickBall(*mpAgent, frontdown, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                            }
                            else{
                                continue;
                            }
                                
                       } 
                       break;
                            
                                
            default:
                    std::cout<<"You can reach here"<<std::endl;
                    return false;
        }
    }

	bool AreSamePoints(Vector A, Vector B, double buffer){
		//Check if and b are the same points +/- buffer
		if( (abs(A.X()-B.X())<buffer) && (abs(A.Y()-B.Y())<buffer))
    		return true;
    	else
    		return false;
	}

	void OccupyHole(Vector target){
	    //Dash to target
	    //while(!AreSamePoints(mpAgent->GetSelf().GetPos(),target,2))
		    //Dasher::instance().GoToPoint(*mpAgent, target, 0.3, 100, true, false);
        mpIntransit = true;
        mpTarget = target;
	}

	double abs(double d){
	    if (d>0.00)
	            return d;
	        else
	            return d*(-1.00);
	}

	Vector RoundToNearestTens(Vector P){
	    // This method rounds a given position to its nearest tens - for example, the rounded position for (12, -17) would be (10, -20)
	    // This helps in locating nearby holes more easily
	    double multX = 10.00;
	    double multY = 10.00;
	    if(P.X()<0.00)
	        multX = -10.00;
	    if(P.Y()<0.00)
	        multY = -10.00;
	    int roundX = static_cast<int>((abs(P.X())+5.00)/10);
	    int roundY = static_cast<int>((abs(P.Y())+5.00)/10);
	    Vector roundedTens = Vector(multX*roundX, multY*roundY);
	    //std::cout<<"Rounded Tens - "<<roundedTens<<std::endl;
	    return roundedTens;
	}

	bool isRTaHole(Vector P){
	    // This method is only for rounded tens
	    // Returns true iff rounded-ten point is a hole    
	    int normalX = static_cast<int>(abs(P.X())/10);
	    int normalY = static_cast<int>(abs(P.Y())/10);
	    if(normalX%2==normalY%2)
	        return true;
	    else
	        return false;
	}

	Vector RoundToNearestHole(Vector P){
	    //std::cout<<"Rounding up point - "<<P<<std::endl;
	    Vector roundedTens = RoundToNearestTens(P);
	    if(isRTaHole(roundedTens)){
	        //std::cout<<"RT is a hole - "<<roundedTens<<std::endl;
	        return roundedTens;
	    }
	    else{
	        Vector roundedHole;
	        double diffX = P.X()-roundedTens.X();
	        double diffY = P.Y()-roundedTens.Y();
	            
	        if(abs(diffX)<abs(diffY)){
	            //Point closer to vertical axis of the diamond
	            if(diffY>0)
	                roundedHole = Vector(roundedTens.X(), roundedTens.Y()+10);
	            else
	                roundedHole = Vector(roundedTens.X(), roundedTens.Y()-10);
	        }
	        else{
	            //Point closer to horizontal axis of the diamond
	            if(diffX>0)
	                roundedHole = Vector(roundedTens.X()+10, roundedTens.Y());
	            else
	                roundedHole = Vector(roundedTens.X()-10, roundedTens.Y());
	        }
	            //std::cout<<"Rounded hole - "<<roundedHole<<std::endl;
	            return roundedHole;
	        }
	}
};

#endif

