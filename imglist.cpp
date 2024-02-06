/**
 *  @file        imglist.cpp
 *  @description Contains partial implementation of ImgList class
 *               for CPSC 221 PA1
 *               Function bodies to be completed by student
 * 
 *  THIS FILE WILL BE SUBMITTED
 */

#include "imglist.h"



#include <math.h> // provides fmax, fmin, and fabs functions

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/**
 * Default constructor. Makes an empty list
 */
ImgList::ImgList(PNG& img) {
    // build the linked node structure and set the member attributes appropriately
    ImgNode* dummy;
    northwest = ImgList::setup_node(0, 0, NULL, img, 2);
    dummy = northwest;
    while(dummy->east  != NULL){
        dummy = dummy -> east;
    }
    while(dummy->south  != NULL){
        dummy = dummy -> south;
    }
    southeast = dummy;
    dummy = NULL;
    width = img.width();
    height = img.height();
}

ImgList::ImgList() {
    // set appropriate values for all member attributes here
    northwest = new ImgNode();
    southeast = new ImgNode();
}





/************
* ACCESSORS *
************/

/**
 * Returns the horizontal dimension of this list (counted in nodes)
 * Note that every row will contain the same number of nodes, whether or not
 *   the list has been carved.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionX() const {
    // replace the following line with your implementatio
    unsigned int count  = 0;
    ImgNode* temp = northwest;
    while(temp == NULL){
        count++;
        temp= temp -> east;

    }
    return count;
}

/**
 * Returns the vertical dimension of the list (counted in nodes)
 * It is useful to know/assume that the grid will never have nodes removed
 *   from the first or last columns. The returned value will thus correspond
 *   to the height of the PNG image from which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   y dimension.
 */
unsigned int ImgList::GetDimensionY() const {
    // replace the following line with your implementation
    unsigned int count  = 0;
    ImgNode* temp = northwest;
    while(temp == NULL){
        count++;
        temp= temp -> south;

    }
    return count;
}

/**
 * Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
 * The returned value will thus correspond to the width of the PNG image from
 *   which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionFullX() const {
    // replace the following line with your implementation
    return width;
}

/**
 * Returns a pointer to the node which best satisfies the specified selection criteria.
 * The first and last nodes in the row cannot be returned.
 * @pre rowstart points to a row with at least 3 physical nodes
 * @pre selectionmode is an integer in the range [0,1]
 * @param rowstart - pointer to the first node in a row
 * @param selectionmode - criterion used for choosing the node to return
 *          0: minimum "brightness" across row, not including extreme left or right nodes
 *          1: node with minimum total of "colour difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 * A note about "brightness" and "colour difference":
 * For PA1, "brightness" will be the sum over the RGB colour channels, multiplied by alpha.
 * "colour difference" between two pixels can be determined
 * using the "distanceTo" function found in RGBAPixel.h.
 */
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
    if (selectionmode ==0) {
        ImgNode* min = new ImgNode();
        min->colour.r = 255;
        min->colour.g = 255;
        min->colour.b = 255;
        min->colour.a = 1;
        ImgNode* temp = min;
        rowstart = rowstart->east;
        while (rowstart->east != NULL) {
            if (rowstart->east->east == NULL) {
                break;
            }
            if (((rowstart->colour.r + rowstart->colour.g + rowstart->colour.b) * rowstart->colour.a) <= ((min->colour.r + min->colour.g + min->colour.b) * min->colour.a)) {
                min = rowstart;
            }
            rowstart = rowstart->east;
        }
        delete temp;
        return min;
    }
    else {
        rowstart = rowstart -> east;
        double min = rowstart->colour.distanceTo(rowstart->east->colour) + rowstart->colour.distanceTo(rowstart->west->colour);
        ImgNode* min_node = rowstart;
        while (rowstart->east->east != NULL) {
            rowstart = rowstart->east;
            double temp = rowstart->colour.distanceTo(rowstart->east->colour) + rowstart->colour.distanceTo(rowstart->west->colour);
            if (temp < min) {
                min = temp;
                min_node = rowstart;
            }
        }
        return min_node;
    }
}

/**
 * Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
 * @pre fillmode is an integer in the range of [0,2]
 * @param fillgaps - whether or not to fill gaps caused by carving
 *          false: render one pixel per node, ignores fillmode
 *          true: render the full width of the original image,
 *                filling in missing nodes using fillmode
 * @param fillmode - specifies how to fill gaps
 *          0: solid, uses the same colour as the node at the left of the gap
 *          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
 *          2: linear gradient between the colour (all channels) of the nodes at the left and right of the gap
 *             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
 *             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
 *             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
 *             Like fillmode 1, use the smaller difference interval for hue,
 *             and the smaller-valued average for diametric hues
 */
PNG ImgList::Render(bool fillgaps, int fillmode) const {
    // Add/complete your implementation below

    PNG outpng(width, height); //this will be returned later. Might be a good idea to resize it at some point.
    if (!fillgaps) {
        ImgNode *temp = northwest;
        unsigned int x = 0;
        unsigned int y = 0;
        while (temp != NULL) {
            ImgNode *temp2 = temp;
            while (temp2 != NULL) {
                RGBAPixel *pixel = outpng.getPixel(x, y);
                pixel->r = temp2->colour.r;
                pixel->g = temp2->colour.g;
                pixel->b = temp2->colour.b;
                pixel->a = temp2->colour.a;

                temp2 = temp2->east;
                x++;
            }
            temp = temp->south;
            y++;
        }
    } else {
        if (fillmode == 1) {
            ImgNode *temp = northwest;
            unsigned int x = 0;
            unsigned int y = 0;
            while (temp != NULL) {
                ImgNode *temp2 = temp;
                while (temp2 != NULL) {

                        RGBAPixel *pixel = outpng.getPixel(x, y);
                        pixel->r = temp2->colour.r;
                        pixel->g = temp2->colour.g;
                        pixel->b = temp2->colour.b;
                        pixel->a = temp2->colour.a;
                        temp2 = temp2->east;
                        x++;
                        if (temp2->skipright != 0){
                            int r = (temp2->colour.r + temp2->east->colour.r)/2;
                            int g = (temp2->colour.g + temp2->east->colour.g)/2;
                            int b = (temp2->colour.b + temp2->east->colour.b)/2;
                            float a = (temp2->colour.a + temp2->east->colour.a)/2;
                            for (int i = 0; i < temp2->skipright; i++){
                                RGBAPixel *pixel = outpng.getPixel(x, y);
                                pixel->r = r;
                                pixel->g = g;
                                pixel->b = b;
                                pixel->a = a;
                                x++;
                            }
                        }
                }
                temp = temp->south;
                y++;
            }
        } else if (fillmode == 0) {
            ImgNode *temp = northwest;
            unsigned int x = 0;
            unsigned int y = 0;
            while (temp != NULL) {
                ImgNode *temp2 = temp;
                while (temp2 != NULL) {
                    for (x; x <= x + temp2->skipright; x++) {
                        RGBAPixel *pixel = outpng.getPixel(x, y);
                        pixel->r = temp2->colour.r;
                        pixel->g = temp2->colour.g;
                        pixel->b = temp2->colour.b;
                        pixel->a = temp2->colour.a;
                    }
                    temp2 = temp2->east;
                }
                temp = temp->south;
                y++;
            }
        } else{
            ImgNode *temp = northwest;
            unsigned int x = 0;
            unsigned int y = 0;
            while (temp != NULL) {
                ImgNode *temp2 = temp;
                while (temp2 != NULL) {

                    RGBAPixel *pixel = outpng.getPixel(x, y);
                    pixel->r = temp2->colour.r;
                    pixel->g = temp2->colour.g;
                    pixel->b = temp2->colour.b;
                    pixel->a = temp2->colour.a;
                    temp2 = temp2->east;
                    x++;
                    if (temp2->skipright != 0){

                        for (int i = 0; i < temp2->skipright; i++){
                            int r = (i + 1) * (temp2->colour.r + temp2->east->colour.r) / (temp2->skipright + 1);
                            int g = (i + 1) * (temp2->colour.g + temp2->east->colour.g) / (temp2->skipright + 1);
                            int b = (i + 1) * (temp2->colour.b + temp2->east->colour.b) / (temp2->skipright + 1);
                            float a = (i + 1) * (temp2->colour.a + temp2->east->colour.a) / (temp2->skipright + 1);
                            RGBAPixel *pixel = outpng.getPixel(x, y);
                            pixel->r = r;
                            pixel->g = g;
                            pixel->b = b;
                            pixel->a = a;
                            x++;
                        }
                    }
                }
                temp = temp->south;
                y++;
            }
        }
    }

    return outpng;
}

/************
* MODIFIERS *
************/

/**
 * Removes exactly one node from each row in this list, according to specified criteria.
 * The first and last nodes in any row cannot be carved.
 * @pre this list has at least 3 nodes in each row
 * @pre selectionmode is an integer in the range [0,1]
 * @param selectionmode - see the documentation for the SelectNode function.
 * @param this list has had one node removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(int selectionmode) {
    // add your implementation here
	
}

// note that a node on the boundary will never be selected for removal
/**
 * Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
 * based on specific selection criteria.
 * Note that this should remove one node from every row, repeated "rounds" times,
 * and NOT remove "rounds" nodes from one row before processing the next row.
 * @pre selectionmode is an integer in the range [0,1]
 * @param rounds - number of nodes to remove from each row
 *        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
 *        i.e. Ensure that the final list has at least two nodes in each row.
 * @post this list has had "rounds" nodes removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(unsigned int rounds, int selectionmode) {
    // add your implementation here
	
}


/*
 * Helper function deallocates all heap memory associated with this list,
 * puts this list into an "empty" state. Don't forget to set your member attributes!
 * @post this list has no currently allocated nor leaking heap memory,
 *       member attributes have values consistent with an empty list.
 */
void ImgList::Clear() {
    ImgNode *temp = northwest;
    ImgNode *temp3 = temp;
    temp = temp->east;

    while (temp -> south != NULL) {

        while (temp != NULL) {
            ImgNode *temp2 = temp;
            temp = temp->east;
            delete temp2;
        }
        temp3 = temp3 -> south;
        temp = temp3 -> east;
        delete temp3 -> north;
    }
}

/**
 * Helper function copies the contents of otherlist and sets this list's attributes appropriately
 * @pre this list is empty
 * @param otherlist - list whose contents will be copied
 * @post this list has contents copied from by physically separate from otherlist
 */
void ImgList::Copy(const ImgList& otherlist) {
    // add your implementation here
	
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist-private.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/

ImgNode* ImgList::setup_node(unsigned int x, unsigned int y, ImgNode* preveous_node, PNG& img, int isHorizontal){
    if(x>= img.width() || y>= img.height()){
        return NULL;
    }

    RGBAPixel *pixel = img.getPixel(x, y);
    ImgNode* list_to_add = new ImgNode();



    //set the colour of the list
    list_to_add -> colour.r = pixel -> r;
    list_to_add -> colour.b = pixel -> b;
    list_to_add -> colour.g = pixel -> g;
    list_to_add -> colour.a = pixel -> a;

    if (x == 0) {
        list_to_add-> west = nullptr;
    }
    if (y == 0) {
        list_to_add->north = nullptr;
    }
    if (isHorizontal == 1) {
        list_to_add->west = preveous_node;
    }
    else if (isHorizontal == 0){
        list_to_add->north = preveous_node;
    }
    
    list_to_add -> east = setup_node(x + 1, y,  list_to_add, img, 1);
    
    list_to_add -> south = setup_node(x, y + 1,  list_to_add, img, 0);

    return list_to_add;
}