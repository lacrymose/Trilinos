#ifndef stk_adapt_UniformRefinerPattern_Quad4_Quad4_4_sierra_hpp
#define stk_adapt_UniformRefinerPattern_Quad4_Quad4_4_sierra_hpp


//#include "UniformRefinerPattern.hpp"
#include <stk_adapt/sierra_element/RefinementTopology.hpp>
#include <stk_adapt/sierra_element/StdMeshObjTopologies.hpp>

#include "UniformRefinerPattern_Line2_Line2_2_sierra.hpp"

namespace stk {
  namespace adapt {

    template <>
    class UniformRefinerPattern<shards::Quadrilateral<4>, shards::Quadrilateral<4>, 4, SierraPort > : public URP<shards::Quadrilateral<4>,shards::Quadrilateral<4>  >
    {

      //Teuchos::RCP< UniformRefinerPattern<shards::Line<2>, shards::Line<2>, 2, SierraPort >  > m_edge_breaker;
      //UniformRefinerPatternBase * m_edge_breaker;
#define EDGE_BREAKER_Q4_Q4_4_S 1
#if EDGE_BREAKER_Q4_Q4_4_S
      UniformRefinerPattern<shards::Line<2>, shards::Line<2>, 2, SierraPort > * m_edge_breaker;
#endif

    public:

      UniformRefinerPattern(percept::PerceptMesh& eMesh, BlockNamesType block_names = BlockNamesType()) :  URP<shards::Quadrilateral<4>, shards::Quadrilateral<4>  >(eMesh)
      {
        m_primaryEntityRank = mesh::Face;
        if (m_eMesh.getSpatialDim() == 2)
          m_primaryEntityRank = mesh::Element;

        setNeededParts(eMesh, block_names, true);
        Elem::StdMeshObjTopologies::bootstrap();

#if EDGE_BREAKER_Q4_Q4_4_S
        if (m_eMesh.getSpatialDim() == 2)
          m_edge_breaker =  new UniformRefinerPattern<shards::Line<2>, shards::Line<2>, 2, SierraPort > (eMesh, block_names) ;
        else
          m_edge_breaker = 0;
#endif

      }

      void setSubPatterns( std::vector<UniformRefinerPatternBase *>& bp, percept::PerceptMesh& eMesh )
      {
        EXCEPTWATCH;
        bp = std::vector<UniformRefinerPatternBase *>(2u, 0);

        if (eMesh.getSpatialDim() == 2)
          {
            bp[0] = this;
#if EDGE_BREAKER_Q4_Q4_4_S
            bp[1] = m_edge_breaker;
#endif
          }
        else if (eMesh.getSpatialDim() == 3)
          {
            // FIXME
            //             std::cout << "ERROR" ;
            //             exit(1);
          }

      }

      virtual void doBreak() {}
      void fillNeededEntities(std::vector<NeededEntityType>& needed_entities)
      {
        needed_entities.resize(2);
        needed_entities[0].first = stk::mesh::Edge;    
        needed_entities[1].first = (m_eMesh.getSpatialDim() == 2 ? stk::mesh::Element : stk::mesh::Face);
        setToOne(needed_entities);
      }

      virtual unsigned getNumNewElemPerElem() { return 4; }

      void 
      createNewElements(percept::PerceptMesh& eMesh, NodeRegistry& nodeRegistry, 
                        Entity& element,  NewSubEntityNodesType& new_sub_entity_nodes, vector<Entity *>::iterator& element_pool,
                        FieldBase *proc_rank_field=0)
      {
        genericRefine_createNewElements(eMesh, nodeRegistry,
                                        element, new_sub_entity_nodes, element_pool,
                                        proc_rank_field);
      }      
    };

  }
}
#endif
