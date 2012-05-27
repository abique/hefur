#ifndef HEFUR_TEMPLATE_FACTORY_HH
# define HEFUR_TEMPLATE_FACTORY_HH

# include <mimosa/singleton.hh>
# include <mimosa/tpl/cache.hh>

namespace hefur
{
  class TemplateFactory : public mimosa::Singleton<TemplateFactory>
  {
  public:
    mimosa::tpl::Template::Ptr create(const char *path);

  private:
    mimosa::tpl::Cache cache_;
  };
}

#endif /* !HEFUR_TEMPLATE_FACTORY_HH */
