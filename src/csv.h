#ifndef CSV_H
#define CSV_H
#include <string>
#include <map>
#include <cmath>
#include <fstream>
#include <set>
#include <iostream>
#include <tuple>
#include <vector>
#include <array>
#ifdef DEBUG
#include <iostream>
#endif
/**
 * Grid class to allow for conversion to a comma separated value file.
 */
class csv{

    std::map<int,std::map<int,std::string> > content;
    int block_size;
    int hc,lc,hr,lr;
    public:
    bool recording=false;
    csv():hc(0),lr(0),lc(0),hr(0),block_size(1){}
    ~csv(){}
    //Set the content of a cell in the CSV
    void setCell(int x,int y,const std::string &i){
        content[y][x]=i;

        //Check for a new maximum bounds and update.
        if(hc<x)
            hc=x+1;
        else if (lc>x)
            lc=x;
        if(lr>y)
            lr=y;
        else if(hr<y)
            hr=y;
    }
    /**
     * Get the content of a cell, or return ' '
     * \param[in] x The x coordinate of the cell to read
     * \param[in] y The y coordinate of the cell to read
     * \returns The value of the cell should it exist, or ' ' if it does not
     */
    std::string getCell(int x,int y)const{
        if(content.find(y)!=content.end())
        {
            auto cc=content.find(y)->second;
            if(cc.find(x)!=cc.end())
                return cc[x];
        }
        return " ";
    }
    /**
     * Get a row from the CSV object
     * \param[in] y The y coordinate of the line to obtain(may be negative)
     */
    std::string getLine(int y)const{
        std::string i;
        for(int c=lc;c<hc;c++){
            i+=getCell(c,y);
            if(c+1<hc)
                i+=",";
        }
        return i;
    }
    void readFromFile(const std::string &input,bool center=false){
	
        //Clear the existing information, should there be any.
        this->content.clear();
        std::ifstream q;
        q.open(input);
        int row=0,column=0;
        char current;
        while(!q.eof()){
            std::string accumulated;
            q.read(&current,1);
            if(current==','){
                setCell(column,row,accumulated);
                //increment the column count
                column++;
                //reset the string
                accumulated="";
            }else if(current=='\n'){
                setCell(column,row,accumulated);
                //increment the row
                row++;
                //reset the column count
                column=0;
                accumulated=""; 
            }else{
                accumulated+=current;
            }
    }
    }
    /**
     * Write the contents of the class as csv as specified by the argument
     * \param[in] output The file which will be written to.
     */
    void writeToFile(const std::string& output)const{
        std::ofstream i;
        i.open(output);
        i<<getContents();
        i.close();
    }
    /**
     * Get the contents of the container, in comma-separated value form.
     */
    std::string getContents() const{
#ifdef DEBUG
        std::cerr<<"Creating string\n";
#endif
        //! \var This is a thing
        std::string c;
        for(int i=lr;i<=hr;i++){
            //add a newline
            c+=getLine(i)+"\n";
        }
        //Yay string!
#ifdef DEBUG
        std::cerr<<"Finished making string\n";
#endif
        return c;
    }
    
    std::vector<std::array<int,4> > stuff;
    
    //simulate a grid with less granularity(E.G. make a single tile designation
    //take up multiple tiles).
    void set_block(int x,int y,unsigned int block_size, const std::string& stuff){
        if(block_size==1)
            //Not so exciting
            this->setCell(x,y,stuff);
        else
            for(int i=0;i<block_size;i++){
                for(int z=0;z<block_size;z++){
                    //Yay Cells
                    this->setCell(x*block_size+i,y*block_size+z,stuff);
                }
            }
    }
    /**
     * Set the cells that make up a line from (x1,y1) to (x2,y2) with blocksize
     * \param[in] x1 the x coordinate of the first point
     * \param[in] y1 the y coordinate of the first point
     * \param[in] x2 the x coordinate of the second point
     * \param[in] y2 the y coordinate of the second point
     * \param[in] b_size the size of the blocks which are set(the total number
     * \param[in] fil  the string to fill each of the affected cells with.
     * of blocks set in any place will be equal to b_size*b_size)
     */
    void linefrom(int x1,int y1,int x2,int y2,
            const std::string &fil,unsigned int b_size=1){
      if(recording){
	std::array<int,4> fff={x1,y1,x2,y2};
	stuff.push_back(fff);
      }
        using namespace std;
        bool steep=abs(y2-y1)>abs(x2-x1);
        if(steep)
        {
            swap(x1,y1);
            swap(x2,y2);
        }
        if(x1>x2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        int deltax=x2-x1;
        int deltay=abs(y2-y1);
        int error=deltax/2;
        int ystep;
        int y=y1;
        if(y1<y2)
            ystep=1;
        else
            ystep=-1;
        for(int x=x1;x<=x2;x++){
            if(steep)
            {
                if(b_size==1)
                    setCell(y,x,fil);
                else
                    set_block(y,x,b_size,fil);
            }else
            {
                if(b_size==1)
                    setCell(x,y,fil);
                else
                    set_block(x,y,b_size,fil);
            }
            error=error-deltay;
            if(error<0){
                y=y+ystep;
                error=error+deltax;
            }
        }
    }
    std::set<std::string> get_extant()const{
      using namespace std;
      set<string> e;
      for(auto i: content){
	for(auto j: i.second){
	  e.insert(j.second);
	}
      }
      return e;
    }
    void writeAsPPM(const std::string &filename,
		    const std::map<std::string, std::tuple<int,int,int> > &cmap)const
    {
      std::ofstream e(filename);
      e<<"P3"<<std::endl;
      e<<hc-lc<<" "<< hr-lr<<std::endl;
      e<<"255"<<std::endl;
      for(int y=lr;y<=hr;y++){
	for(int x=lc;x<=hc;x++){
	  if(cmap.find(this->getCell(x,y))==cmap.end()){
	    e<<"0 0 0"<<std::endl;
	  }else{
	  std::tuple<int,int,int> c=cmap.find(this->getCell(x,y))->second;
	  e<<std::get<0>(c)<<" "<<std::get<1>(c)<<" "<<std::get<2>(c)<<" ";
	  }
	}
	e<<std::endl;
      }
      e.close();
    }
    int width()const{
      return hc-lc;
    }
    int height()const{
      return hr-lr;
    }
    std::array<int,4> getDim()const{
      return std::array<int,4>{lc,lr,hc,hr};
    }
};
#endif
