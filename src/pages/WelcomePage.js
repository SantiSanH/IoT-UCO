import React from 'react'
import '../components/styles/StylesWelcome.css'
import Typography from '@material-ui/core/Typography'
import Box from '@material-ui/core/Box'

function Welcome() {

    return (
        <div className='container'>
            <Box my={4}>
                <Typography gutterBottom>
                    Hello Luis Miguel, how are you?
                        </Typography>
                <Typography gutterBottom>
                    welcome to the login in this proyect!!
                        </Typography>
            </Box>

        </div>


    )


}

export default Welcome