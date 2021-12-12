BEGIN { lif = 0
        h = 0
      }
{ if( $1 !~ "^%" )
  { if( h == 0 )
    { h++
      print NR, "T"
    }
    else if( h == 1 )
    { h++
      if( h == 2 && $1 !~ "%h" )
        print NR, "H"
    }
  }
  else if( $1 ~ "%input" )
    h = 2
  else if( $1 ~ "%if" )
    hif[++lif] = h
  else if( $1 ~ "%elif" )
    h = hif[lif];
  else if( $1 ~ "%else" )
    h = hif[lif];
  else if( $1 ~ "%exit" )
    h = hif[lif];
  else if( $1 ~ "%fi" )
    lif--;
  else if( $1 ~ "%parameter" || $1 ~ "%menuhead" )
    hpar = h
  else if( $1 ~ "%menu" )
    h = hpar;
  else if( $1 ~ "%end" )
    h = hpar;
  else if( $1 ~ "%switch" )
    hswi = h;
  else if( $1 ~ "%case" )
    h = hswi;
  else if( $1 ~ "%default" )
    h = hswi;
  else if( $1 !~ "%button" && $1 !~ "%nonl" && $1 !~ "%table" && $1 !~ "%mysqlquery" && $1 !~ "%setoben" && $1 !~ "%setrechts" && $1 !~ "%mysqlwrite" && $1 !~ "%include" && $1 !~ "%left" && $1 !~ "^%%" && $1 !~ "%resultpage" && $1 !~ "%okbutton" && $1 !~ "%eval" && $1 !~ "%for" && $1 !~ "%while" && $1 !~ "%/" && $1 !~ "%let" && $1 !~ "%var" )
  { if( h == 1 )
    { h++
      if( h == 2 && $1 !~ "%h" )
        print NR, "H"
    }
  }
}
