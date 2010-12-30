#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include <QString>
using namespace std;

class Tile
{
    public:
        Tile(string type);
        int getHeight() const {return data.size();}
        int getWidth() const {return data.size()>0?data[0].size():0;}
        void rotateTile();
        void reflectTile();
        int score() const;
        vector<vector<char> > data;
        string getAsText() const;
        QString getAsQString() const {return QString(getAsText().c_str());}
        int scor;
};

ostream& operator<<(ostream&,const Tile&);

#endif
