import React from 'react'
import { BrowserRouter, Route, Switch} from 'react-router-dom'
import MainPage from '../pages/MianPage'
import Welcome from '../pages/WelcomePage'


const App = () => (                                                     //de esta manera funciona igual que usando
                                                                        //function APP comentado abajo, es una manera mas
                                                                        //moderna de trabajar javascript
   <div>                                                                   
                                                                    
    <BrowserRouter>                                                                   
        <Switch>
                <Route exact path = "/" component = {MainPage}/>
                <Route exact path = "/welcome" component = {Welcome}/>
        </Switch>
    </BrowserRouter>
    </div> 
)

export default App
