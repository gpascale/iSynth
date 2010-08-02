/*
 *  PSMasterOldCrap.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/26/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#if 0

void
PSMaster::getRandomQuad(Quad &q,
								float posLo, float posHi,
								float rotLo, float rotHi,
								float scaleLo, float scaleHi,
								string textureFile)
{
	Vector3 randPos(0.0f, 0.0f, 0.0f);
	Vector3 randScale(1.0f, 1.0f, 1.0f);
	Vector3 randRot(0.0f, 0.0f, 0.0f);
	for(int j = 0; j < 3; j++) randPos[j] = randFloat(posLo, posHi);
	for(int j = 0; j < 3; j++) randRot[j] = randFloat(rotLo, rotHi);
	for(int j = 0; j < 3; j++) randScale[j] = randFloat(scaleLo, scaleHi);
	
	q.setPosition(randPos);
	q.setRotation(randRot);
	q.setScale(randScale);
	
	float r = randFloat(0.0f, 1.0f);
	float g = randFloat(0.0f, 1.0f);
	float b = randFloat(0.0f, 1.0f);
	
	cout << "r = " << r << " g = " << g << " b = " << b << endl;
	
	Color4F randColor(r, g, b, 0.0f);
	//q.setColor(randColor);
	
	if(textureFile == "random"){
		textureFile = "images/Apartment_Small/Apartment " 
		+ i2s(randInt(1, 12), 3) + ".jpg";
	}
	q.setTextureFile(textureFile);
    
}

void
PSMaster::goToRandomNeighbor()
{
    JsonCollection *coll      = m_dataManager->getCollection();
    const JsonCoordSystem &c = coll->getCoordSystem(m_curCoordSystem);   
    
    const JsonProjector &p = c.getProjector(m_curProjector);
    JsonProjector * const * projs = p.getAllNeighbors();
    
    while(1){
        int r = rand() % 10;
        if(projs[r] == NULL) continue;        
        goToImageByCSAndProjectorIndex( projs[r]->getCoordSystem().getIndex(),
                                       projs[r]->getLocalIndex());		
        break;
    }
}

const string&
PSMaster::getURLFromCSAndProjectorIndex(int iCoordSystem, int jProjector) const
{		
	return m_scene->getQuad(iCoordSystem, jProjector).getBaseURL();	
}

void
PSMaster::goToRandomImage()
{
    JsonCollection *coll      = m_dataManager->getCollection();
    const JsonCoordSystem &c  = coll->getCoordSystem(m_curCoordSystem);    
    
    goToImageByCSAndProjectorIndex(m_curCoordSystem, rand() % (int) c.getNumProjectors());
}



void
PSMaster::turnOffAutoRotate()
{
    
    static const int NUM_FRAMES = 700;
    const vector<Drawable*> &entities = m_scene->getEntities(0);
    int numEntities = (int) entities.size();
    
    vector<Vector3> dRots(numEntities);
    for(int i = 0; i < numEntities; i++){
        Vector3 rot;
        entities[i]->getRotation(rot);
        dRots[i] = (-rot) / (float) NUM_FRAMES;
    }
    
    for(int i = 0; i < NUM_FRAMES; i++){
        for(int j = 0; j < numEntities; j++){
            entities[j]->rotate(dRots[j]);
        }
        m_scene->render(m_curCoordSystem);
    }
    
    m_autorotate = false;
    
}


void
PSMaster::turnOnAutoRotate()
{
     const vector<Drawable*> &entities = m_scene->getEntities(0);
     int numEntities = (int) entities.size();
     m_rotVectors.resize(numEntities);
     for(int i = 0; i < numEntities; i++){
         m_rotVectors[i].x = randFloat(.001f, .0049f);
         m_rotVectors[i].y = randFloat(.001f, .0049f);
         m_rotVectors[i].z = randFloat(.001f, .0049f);
     }
     m_autorotate = true;
}


void
PSMaster::toggleAutoRotate()
{
    if(m_autorotate)
        turnOffAutoRotate();
    else
        turnOnAutoRotate();
}


void
PSMaster::enter2DMode(int numFrames,
							  bool moveCamera,
							  const vector<Vector3> &gPos)
{
    
     // hacky and stupid, but it works
     static Vector3 gridPositions[35];
     
     if(!gPos.empty()){
         for(int i = 0; i < (int) gPos.size(); i++) gridPositions[i] = gPos[i];
     }	
     else{
         float r, c;
         int i, j;
         int n = 0;
         for(i = 0, r = 2.0f; i < 5; i++, r -= 1.0f){
             for(j = 0, c = -3.0f; j < 7; j++, c += 1.0f){
                 gridPositions[n].x = c;
                 gridPositions[n].y = r;
                 gridPositions[n].z = 0.0f;
                 n++;
             }
         }
     }
     
     const vector<Drawable*> &entities = m_scene->getEntities(0);
     int numEntities = (int) entities.size();
     
     if((int) entities.size() > 35){
         cout << "Whoops, can't enter 2d mode with more than 35 images in this version" << endl;
         return;
     }    
     
     vector<Vector3> dRot((int) numEntities);
     vector<Vector3> dPos((int) numEntities);
     vector<Vector3> dScale((int) numEntities);
     
     float invNumFrames = 1.0f / numFrames;
     static Vector3 objPos, objRot, objScale;    
     static Drawable *Drawable;
     
     for(int i = 0; i < numEntities; i++){
         Drawable = entities[i];
         Drawable->getPosition(objPos);
         Drawable->getRotation(objRot);
         Drawable->getScale(objScale);        
         dPos[i] = invNumFrames * (gridPositions[i] - objPos);
         dRot[i] = invNumFrames * (-objRot);
         dScale[i] = invNumFrames * ((-objScale) + Vector3(0.95f, 0.95f, 0.95f));
     }
        
     Vector3 camPos;
     m_camera->getPosition(camPos);
     Vector3 dCamPos = invNumFrames * (Vector3(0.0f, 0.0f, -5.0f) - camPos);    
        
     for(int i = 0; i < numFrames; i++){
         for(int j = 0; j < numEntities; j++){
             entities[j]->translate(dPos[j]);                        
             Vector3 scale;            
             entities[j]->getScale(scale);
             entities[j]->setScale(scale + dScale[j]);
             entities[j]->rotate(dRot[j]);                       
         }
         if(moveCamera)
             m_camera->translate(dCamPos);
         m_scene->render(m_curCoordSystem); 
     }
     
}


void
PSMaster::enter3DMode(int numFrames, bool moveCamera)
{

     // hacky and stupid, but it works
     static Vector3 rPositions[35];
     static Vector3 rRotations[35];
     static Vector3 rScales[35];
     
     const vector<Drawable*> &entities = m_scene->getEntities(0);
     int numEntities = (int) entities.size();
     
     for(int i = 0; i < numEntities; i++){
         for(int j = 0; j < 3; j++) rPositions[i][j] = randFloat(-15.0f, 15.0f);
         for(int j = 0; j < 3; j++) rRotations[i][j] = randFloat(-20.0f, 20.0f);
         for(int j = 0; j < 3; j++) rScales[i][j] = randFloat(1.0f, 4.0f);
     }
     
     vector<Vector3> dRot((int) numEntities);
     vector<Vector3> dPos((int) numEntities);
     vector<Vector3> dScale((int) numEntities);
     
     float invNumFrames = 1.0f / numFrames;
     static Vector3 objPos, objRot, objScale;    
     static Drawable *Drawable;
     
     for(int i = 0; i < numEntities; i++){
         Drawable = entities[i];
         Drawable->getPosition(objPos);
         Drawable->getRotation(objRot);
         Drawable->getScale(objScale);        
         dPos[i] = invNumFrames * (rPositions[i] - objPos);
         dRot[i] = invNumFrames * (rRotations[i] - objRot);
         dScale[i] = invNumFrames * (rScales[i] - objScale);
     }
     
     Vector3 camPos;
     m_camera->getPosition(camPos);
     Vector3 dCamPos = invNumFrames * (Vector3(0.0f, 0.0f, -25.0f) - camPos);    
     
     for(int i = 0; i < numFrames; i++){
         for(int j = 0; j < numEntities; j++){
             entities[j]->translate(dPos[j]);                        
             Vector3 scale;            
             entities[j]->getScale(scale);
             entities[j]->setScale(scale + dScale[j]);
             entities[j]->rotate(dRot[j]);                       
         }
         if(moveCamera)
             m_camera->translate(dCamPos);
         m_scene->render(m_curCoordSystem);
     }

}


void doDraw()
{    
    
    const GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
    const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
    
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumf(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, 1000.0f);
    /*
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
     */
    
    
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // m_camera->setLens(1.0f, 1.0f, 0.1f, 1000.0f, 0.5f * PI);        
    
	//m_scene->begin(false);
    
    Vertex::XYZ v[3];
    v[0] = Vertex::XYZ(-1.0f, -1.0f, 0.0f);
    v[1] = Vertex::XYZ(1.0f, -1.0f, 0.0f);
    v[2] = Vertex::XYZ(0.0f, 1.0f, 0.0f);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, v);  
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    //   glMatrixMode(GL_PROJECTION);    
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    
}



// part of updateControlButtons
					  	
    if(m_rightButton){
        if(curProj.hasRightNeighbor()){
            m_rightButton->setOpacity(ifThere, duration);
            if(!deactivateAll)
                m_rightButton->setClickable(true);
                else
                    m_rightButton->setClickable(false);
                    }
        else{
            m_rightButton->setOpacity(ifNot, duration);
            m_rightButton->setClickable(false);
        }
    }

    if(m_upButton){
        if(curProj.hasUpNeighbor()){
            m_upButton->setOpacity(ifThere, duration);
            if(!deactivateAll)
                m_upButton->setClickable(true);
                else
                    m_upButton->setClickable(false);
                    }
        else{
            m_upButton->setOpacity(ifNot, duration);
            m_upButton->setClickable(false);
        }
    }

    if(m_downButton){
        if(curProj.hasDownNeighbor()){
            m_downButton->setOpacity(ifThere, duration);
            if(!deactivateAll)
                m_downButton->setClickable(true);
                else
                    m_downButton->setClickable(false);
                    }
        else{	
            m_downButton->setOpacity(ifNot, duration);
            m_downButton->setClickable(false);
        }
    }

    if(m_inButton){
        if(curProj.hasInNeighbor()){
            m_inButton->setOpacity(ifThere, duration);
            if(!deactivateAll) 
                m_inButton->setClickable(true);
                else
                    m_inButton->setClickable(false);
                    }
        else{
            m_inButton->setOpacity(ifNot, duration);
            m_inButton->setClickable(false);
        }
    }

    if(m_outButton){
        if(curProj.hasOutNeighbor()){
            m_outButton->setOpacity(ifThere, duration);
            if(!deactivateAll) 
                m_outButton->setClickable(true);
                else
                    m_outButton->setClickable(false);
                    }
        else{
            m_outButton->setOpacity(ifNot, duration);
            m_outButton->setClickable(false);
        }
    }	
 

// part of updatePanorama
    for(iter = beg, idx = 0; iter != end; iter++, idx++){	
        OpacitySpring& oSpr = getOpacitySpring(m_curCoordSystem, *iter);				
        if(abs(idx - best) <= 2){
            const float angAndPri = (idx == best) ? bestAngle : (0.5f * bestAngle);
            if(oSpr.getTargetOpacity() != angAndPri){
                oSpr.setDuration(1.0f);
                oSpr.setTargetOpacity(angAndPri);
                oSpr.start(m_now);
            }
            oSpr.setTargetOpacity(angAndPri);
            makeImageVisible(m_curCoordSystem, *iter, angAndPri);
            m_textureUpdateSet.insert(TextureUpdate(m_curCoordSystem, 
                                                    *iter,
                                                    TEXTURE_UPDATE_UPGRADE));
        }
        else{
            m_textureUpdateSet.insert(TextureUpdate(m_curCoordSystem, 
                                                    *iter,
                                                    TEXTURE_UPDATE_DOWNGRADE));
            if(oSpr.getTargetOpacity() != 0.0f){
                oSpr.setDuration(1.0f);
                oSpr.setTargetOpacity(0.0f);
                oSpr.start(m_now);
            }
        }			
    }


void
PSMaster::leavePanoramaMode()
{
    APP_PRINTF("leavePanoramaMode\n");
    const JsonCollection *coll = m_dataManager->getCollection();
    const JsonCoordSystem &cs = coll->GetCoordSystem(m_curCoordSystem);
    const Panorama& curPano = cs.getPanorama(m_panorama);
    
    Panorama::const_iterator iter;
    const Panorama::const_iterator beg = curPano.begin();
    const Panorama::const_iterator end = curPano.end();
    
    for(iter = beg; iter != end; iter++){
        const OpacitySpring &spr = getOpacitySpring(m_curCoordSystem, *iter);
        spr.setTargetOpacity(0.0f);
        spr.setDuration(0.5f);
        spr.start();
        makeImageInvisible(m_curCoordSystem, *iter);
    }
    //m_panorama = -1;
}


void
PSMaster::enterPanoramaMode(int panorama)
{
    APP_PRINTF("enterPanoramaMode(%d)\n", panorama);

    m_panorama = panorama;

    const JsonCollection *coll = m_dataManager->getCollection();
    const JsonCoordSystem &cs = coll->GetCoordSystem(m_curCoordSystem);
    const Panorama& curPano = cs.getPanorama(m_panorama);

    Panorama::const_iterator iter;
    const Panorama::const_iterator beg = curPano.begin();
    const Panorama::const_iterator end = curPano.end();

    for(iter = beg; iter != end; iter++)
        if(*iter != m_curProjector)					
            makeImageInvisible(m_curCoordSystem, *iter);

    m_curPanoramaImg = m_curProjector;

}

void
PSMaster::doAnimation()
{	
	//vector<Drawable*> &entities = m_scene->getEntities(0);
	//entities.clear();			
	
	Quad q;
	for(int i = 0; i < 35; i++){
		getRandomQuad(q, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, "");
		m_scene->addQuad(0, q);
	}
	
    
	m_camera->setOrientation(Vector3(0.0f, 0.0f, -5.0f),
							 Vector3(0.0f, 0.0f, 1.0f),
							 Vector3(0.0f, 1.0f, 0.0f));				
    
	turnOffAutoRotate();
	for(int i = 0; i < 150; i++) update();		
	enter3DMode(randInt(1500, 3000));
	turnOnAutoRotate();
	for(int i = 0; i < 1600; i++) update();
	turnOffAutoRotate();
	for(int i = 0; i < 1500; i++) update();
	turnOnAutoRotate();
    
	static Vector3 position, look;
	position = m_camera->getPosition();
	Matrix rot;
	Matrix::RotationMatrix(rot, 0.002f, Vector3(0.0f, 1.0f, 0.0f));		
	for(int i = 0; i < 3000; i++)
	{
		position = rot * position;
		look = -position;
		look.normalize();
		m_camera->setOrientation(position,
								 look,
								 Vector3(0.0f, 1.0f, 0.0f));
		update();
		m_camera->moveBackward(0.003f);
	}
    
	Matrix::RotationMatrix(rot, -0.002f, Vector3(0.0f, 1.0f, 0.0f));		
	for(int i = 0; i < 3000; i++)
	{
		position = rot * position;
		look = -position;
		look.normalize();
		m_camera->setOrientation(position,
								 look,
								 Vector3(0.0f, 1.0f, 0.0f));
		update();
		m_camera->moveForward(0.003f);
	}
    
	turnOffAutoRotate();
	turnOnAutoRotate();
	for(int i = 0; i < 1000; i++) update();
    
	Matrix::RotationMatrix(rot, 0.002f, Vector3(1.0f, 0.0f, 0.0f));		
	for(int i = 0; i < 3000; i++)
	{
		position = rot * position;
		look = -position;
		look.normalize();
		m_camera->setOrientation(position,
								 look,
								 look.cross(Vector3(1.0f, 0.0f, 0.0f)));
		update();
		m_camera->moveBackward(0.01f);
	}
    
	Matrix::RotationMatrix(rot, -0.002f, Vector3(1.0f, 0.0f, 0.0f));		
	for(int i = 0; i < 3000; i++)
	{
		position = rot * position;
		look = -position;
		look.normalize();
		m_camera->setOrientation(position,
								 look,
								 look.cross(Vector3(1.0f, 0.0f, 0.0f)));
		update();
		m_camera->moveForward(0.01f);
	}
    
	for(int i = 0; i < 1300; i++){
		m_camera->moveBackward(0.1f);
		update();
	}
	enter2DMode(randInt(5000, 6000));
	turnOffAutoRotate();
	for(int i = 0; i < 2500; i++) update();
	turnOnAutoRotate();
	for(int i = 0; i < 1000; i++) update();
	turnOffAutoRotate();
	for(int i = 0; i < 2500; i++) update();
	turnOnAutoRotate();
    
	for(int i = 0; i < 500; i++){
		m_camera->moveBackward(0.005f);
		update();
	}
	position = m_camera->getPosition();
    
	Matrix::RotationMatrix(rot, 0.002f, Vector3(0.0f, 1.0f, 0.0f));		
	for(int i = 0; i < 4000; i++)
	{
		position = rot * position;
		look = -position;
		look.normalize();
		m_camera->setOrientation(position,
								 look,
								 Vector3(0.0f, 1.0f, 0.0f));
		update();
		m_camera->moveBackward(0.003f);
	}
    
	Matrix::RotationMatrix(rot, -0.002f, Vector3(0.0f, 1.0f, 0.0f));		
	for(int i = 0; i < 4000; i++)
	{
		position = rot * position;
		look = -position;
		look.normalize();
		m_camera->setOrientation(position,
								 look,
								 Vector3(0.0f, 1.0f, 0.0f));
		update();
		m_camera->moveForward(0.003f);
	}
    
	for(int i = 0; i < 1600; i++){
		m_camera->moveBackward(0.02f);
		m_camera->rollLeft(0.004f);
		update();
	}
	
	for(int i = 0; i < 1600; i++){
		m_camera->moveForward(0.02f);
		m_camera->rollRight(0.004f);
		update();
	}
	
	for(int i = 0; i < 200; i++) update();
	
	Vector3 dir(0.003f, 0.0f, 0.003f);
	for(int i = 0; i < 1900; i++){
		update();
		m_camera->translate(dir);
	}							
	
	m_camera->translate(Vector3(-500 * 0.03f, 0.0f, 20.0f));
	
	enter3DMode(1, false);
    
	enter2DMode(2000);
	
	for(int i = 0; i < 3000; i++) update();
	
	vector<Vector3> pos(35, Vector3(0.0f, 0.0f, randFloat(-2.0f, 2.0f)));
	enter2DMode(randInt(500, 1000), true, pos);
    
}


void
PSMaster::takeTour()
{
	
    JsonCollection *coll = m_dataManager->getCollection();
    
    static  CoordSystems    coordSystems;
    static  Projectors      projectors;
    static  JsonImages      images;
    
    coordSystems    = coll->GetCoordSystems();        
    images          = coll->GetImages();
    
    
    projectors = coordSystems[m_curCoordSystem]->getProjectors();
    for(int i = 0; i < (int) projectors.size(); i++){
        Drawable &e = m_scene->getQuad(m_curCoordSystem, i);
        e.setWireframe(true);
    }
    for(int i = 0; i < (int) projectors.size(); i++){
        
        JsonProjector *p = projectors[i];
        const Vector3 &campos = p->getPosition();
        const Quaternion &rot = p->getRotation();
        cout << "drawing image " << p->getImageIndex() << endl;
        cout << "placing the camera at " << campos << endl;
        m_camera->setPosition(campos);
        m_camera->setRotation(rot);
        
        Drawable &e = m_scene->getQuad(m_curCoordSystem, i);
        e.setWireframe(false);
        for(int j = 0; j < 200; j++)
            m_scene->render(m_curCoordSystem, m_now);
        e.setWireframe(true);
    }    
}


#endif // 0
