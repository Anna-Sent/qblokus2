#include "tile.h"
#include <iostream>
using namespace std;

Tile::Tile(string src)
{
    data.resize(1);
    scor=0;
    for(size_t i = 0; i<src.size(); ++i)
    {
        if(src[i]=='|')
        {
            data.resize(data.size()+1);
        }
        else
        {
            data.back().push_back(src[i]);
            if (src[i]!='0') ++scor;
        }
    }
    //    rotateTile();
}

void Tile::rotateTile()
{
    //    cout << "rotate\n";
    vector<vector<char> > newdata(getWidth(),vector<char>(getHeight(),'0'));
    for(int i = 0;i<getHeight();++i)
    {
        for(int j = 0;j<getWidth();++j)
        {
            newdata[j][getHeight()-i-1]=data[i][j];
        }
    }
    data.swap(newdata);
}

int Tile::score() const
{
    return scor;
}

void Tile::reflectTile()
{
    //    cout << "rotate\n";
    vector<vector<char> > newdata(getHeight(),vector<char>(getWidth(),'0'));
    for(int i = 0;i<getHeight();++i)
    {
        for(int j = 0;j<getWidth();++j)
        {
            newdata[i][getWidth()-j-1]=data[i][j];
        }
    }
    data.swap(newdata);
}

string Tile::getAsText() const
{
    string result;
    for(int i = 0;i<getHeight();++i)
    {
        for(int j = 0;j<getWidth();++j)
        {
            result.push_back(data[i][j]);
        }
        if (i<getHeight()-1) result.push_back('|');
    }
    return result;
}

ostream& operator<<(ostream& to,const Tile& what)
{
    for(size_t i=0;i<what.data.size();++i)
    {
        for(size_t j=0;j<what.data[i].size();++j)
        {
            to<<what.data[i][j];
        }
        to<<"\n";
    }
    return to;
}

#ifdef TEST
int main()
{
    Tile tile("12|01");
    cout << tile;
}
#endif
