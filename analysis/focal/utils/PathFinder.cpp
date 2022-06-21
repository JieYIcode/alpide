#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TCollection.h"

void PathFinder(std::string dirname, std::string head, std::string ext, std::vector<std::string> &paths);

void PathFinder(std::string dirname, std::string head, std::string ext, std::vector<std::string> &paths) {
    dirname+="/";
    TSystemDirectory dir(dirname.c_str(), dirname.c_str());
    TList *files = dir.GetListOfFiles();
    //std::cout << "Looking for " << dirname << "/" << head << "*"<<ext << " files in a list " << files->GetEntries() << " files" << std::endl;
    if (files) { 
        TSystemFile *file;
        TString fname; 
        TIter next((TCollection*) files);
        while ((file=(TSystemFile*)next())) {
            fname = file->GetName();
            if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && fname.BeginsWith(head.c_str())) {
                //cout << dirname<<fname.Data() << endl; 
                paths.push_back(dirname+fname.Data());
            }
            else if(file->IsDirectory() && fname!="." && fname!=".."){
                //std::cout<<fname.Data()<<std::endl;
                PathFinder(dirname+fname.Data(), head, ext, paths);
            }
        }
    } 
    //return paths;
}

