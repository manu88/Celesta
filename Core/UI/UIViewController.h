//
//  UIViewController.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 09/12/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UIViewController_hpp
#define UIViewController_hpp

class UIView;


class UIViewController
{
    friend class UIView;
public:
    virtual ~UIViewController()
    {}
    
protected:
    
    virtual void viewWillAppear   ( UIView *view );
    virtual void viewDidAppear    ( UIView *view );
    virtual void viewWillDisappear( UIView *view );
    virtual void viewDidDisappear ( UIView *view);

    
    UIViewController() {}
};

#endif /* UIViewController_hpp */
