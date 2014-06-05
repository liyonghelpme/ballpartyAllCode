//
//  MatchService.h
//  ballparty
//
//  Created by dozengame on 14-5-24.
//
//

#ifndef __ballparty__MatchService__
#define __ballparty__MatchService__

#include <iostream>
#include "ServiceBase.h"
class Match;

class MatchService : public ServiceBase{
public:
    bool getMatches(const double startTime, const double endTime);
    Match *getMatchById(int id);
    
    
    MatchService(string url);
    

    bool getReqYet() {
        return m_requestYet;
    }
    bool getInitYet() {
        return m_initMatchYet;
    }
    vector<Match*> &getAllMatch() {
        return m_matchList;
    }
    
    void clearTestNum(){
        m_testNum = 0;
    }
    
    //获取旧的比赛信息
    bool fetchOldMatchInfo();
    
    //新获取的 数据的开始和结束索引
    int startInd;
    int endInd;
    
    
    //获取新的比赛信息
    bool fetchNewMatchInfo();
    bool getNetError() {
        return netError;
    }
    
    void clearState();
    
    
private:
    void onUpdate(float);
    
    bool netError;
    float updateTime;
    
    
    bool noOld, noNew;
    
    bool realFetchNewMatch();
    bool realFetchOldMatch();
    
    bool m_initMatchYet;
    bool m_requestYet;
    vector<Match*> m_matchList;
    
    void initMatchOver(bool isSuc, string s, void *param);
    void initOldMatchOver(bool isSuc, string s, void *param);
    void initNewMatchOver(bool isSuc, string s, void *param);
    
    bool m_searchYet;
    int m_testNum;
    double m_firstDay, m_lastDay, m_dayRange;
    
    
    void realRequestMatch();
};


#endif /* defined(__ballparty__MatchService__) */
